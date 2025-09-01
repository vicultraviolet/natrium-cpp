#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vSwapchainRenderTarget.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

#include "Internal.hpp"

#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"

namespace Na::VulkanImpl {
	static vk::SurfaceFormatKHR pickSurfaceFormat(const Na::ArrayList<vk::SurfaceFormatKHR>& formats)
	{
		for (auto it = formats.begin(); it != formats.end(); it++)
			if (it->format == vk::Format::eR8G8B8A8Uint
				&& it->colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return *it;
		return formats[0];
	}

	static vk::PresentModeKHR pickPresentMode(const Na::ArrayList<vk::PresentModeKHR>& present_modes)
	{
		for (auto it = present_modes.begin(); it != present_modes.end(); it++)
			if (*it == vk::PresentModeKHR::eMailbox)
				return *it;
		return vk::PresentModeKHR::eFifo;
	}

	static vk::Extent2D pickResolution(const vk::SurfaceCapabilitiesKHR& capabilities, u32 window_width, u32 window_height)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;

		return {
			std::clamp<u32>(window_width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp<u32>(window_height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
		};
	}

	SwapchainRenderTarget::SwapchainRenderTarget(
		WeakRef<const Window> window,
		Ref<const RendererSettingsAsset> renderer_settings
	)
	: m_Window(window),
	  m_RendererSettings(renderer_settings),
	  m_Frames(init::init, renderer_settings->max_frames_in_flight())
	{
		this->_create_window_surface();
		this->_create_swapchain();
		this->_create_image_views();
		this->_create_color_buffer();
		this->_create_depth_buffer();
		this->_create_render_pass();
		this->_create_framebuffers();
		this->_create_sync_objects();
	}

	void SwapchainRenderTarget::destroy(void)
	{
		const auto& logical_device = Device::Get()->logical_device();

		for (FrameData& fd : m_Frames)
		{
			logical_device.destroyFence(fd.in_flight_fence);

			logical_device.destroySemaphore(fd.image_available_semaphore);
			logical_device.destroySemaphore(fd.render_finished_semaphore);
		}
		m_Frames.destroy();

		for (vk::Framebuffer framebuffer : m_Framebuffers)
			logical_device.destroyFramebuffer(framebuffer);
		m_Framebuffers.destroy();

		if (m_RenderPass)
		{
			logical_device.destroyRenderPass(m_RenderPass);
			m_RenderPass = nullptr;
		}

		m_DepthImage.destroy();
		m_ColorImage.destroy();

		for (auto& img_view : m_ImageViews)
			logical_device.destroyImageView(img_view);
		m_ImageViews.destroy();

		m_Images.destroy();

		if (m_Swapchain)
		{
			logical_device.destroySwapchainKHR(m_Swapchain);
			m_Swapchain = nullptr;
		}

		if (m_Surface)
		{
			Device::Get()->instance().destroySurfaceKHR(m_Surface);
			m_Surface = nullptr;
		}

		m_Window = nullptr;
	}

	bool SwapchainRenderTarget::acquire_next_image(void)
	{
		const auto& logical_device = Device::Get()->logical_device();
		const FrameData& fd = m_Frames[m_FrameIndex];

		if (auto window = m_Window.lock())
		{
			if (window->width()  != m_Width ||
				window->height() != m_Height)
			{
				this->recreate_swapchain();
				return false;
			}
		} else
		{
			return false;
		}

		vk::Result result{};

		result = logical_device.waitForFences(
			{ fd.in_flight_fence },
			VK_TRUE, // wait all
			UINT64_MAX // timeout
		);
		NA_VERIFY_VK(
			result,
			"Failed to begin frame #{} with image #{}:"
			"Error in waiting for fence!",
			m_FrameIndex,
			m_ImageIndex
		);

		result = logical_device.acquireNextImageKHR(
			m_Swapchain,
			UINT64_MAX, // timeout
			fd.image_available_semaphore,
			nullptr, // fence
			&m_ImageIndex
		);

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			this->recreate_swapchain();
			return false;
		} else
		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			throw std::runtime_error("Failed to acquire next swapchain image!");

		if (m_ImageInFlightFences[m_ImageIndex])
		{
			result = logical_device.waitForFences(
				{ m_ImageInFlightFences[m_ImageIndex] },
				VK_TRUE,
				UINT64_MAX
			);
			NA_VERIFY_VK(result, "Failed to acquire next swapchain image: Error in waiting for fence!");
		}
		m_ImageInFlightFences[m_ImageIndex] = fd.in_flight_fence;

		result = logical_device.resetFences(1, &fd.in_flight_fence);
		NA_VERIFY_VK(result, "Failed to acquire next swapchain image: Error in resetting fence!");

		return true;
	}

	void SwapchainRenderTarget::begin_render_pass(
		vk::CommandBuffer cmd_buffer,
		const glm::vec4& color = Colors::k_Black
	) const
	{
		vk::RenderPassBeginInfo render_pass_info;

		render_pass_info.renderPass = m_RenderPass;
		render_pass_info.framebuffer = m_Framebuffers[m_ImageIndex];

		render_pass_info.renderArea.offset = { { 0, 0 } };
		render_pass_info.renderArea.extent = vk::Extent2D(m_Width, m_Height);

		std::array<vk::ClearValue, 2> clear_values;
		clear_values[0].color = std::array<float, 4>{ color.r, color.g, color.b, color.a };
		clear_values[1].depthStencil = { { 1.0f, 0 } };

		render_pass_info.clearValueCount = (u32)clear_values.size();
		render_pass_info.pClearValues = clear_values.data();

		cmd_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

		cmd_buffer.setViewport(0, 1, &m_ViewportVk);
		cmd_buffer.setScissor(0, 1, &m_ScissorVk);
	}

	void SwapchainRenderTarget::present(void)
	{
		const FrameData& fd = m_Frames[m_FrameIndex];

		vk::PresentInfoKHR present_info;

		present_info.setWaitSemaphores({ fd.render_finished_semaphore });
		present_info.setSwapchains({ m_Swapchain });

		present_info.pImageIndices = &m_ImageIndex;

		try
		{
			vk::Result result = Device::Get()->graphics_queue().presentKHR(present_info);
			switch (result)
			{
			case vk::Result::eSuboptimalKHR:
				this->recreate_swapchain();
				break;
			case vk::Result::eErrorOutOfDateKHR:
				this->recreate_swapchain();
				break;
			case vk::Result::eSuccess:
				break;
			default:
				NA_VERIFY_VK(result, "Error in presenting to graphics queue!");
			}

		} catch (const vk::OutOfDateKHRError& err)
		{
			(void)err;
			this->recreate_swapchain();
		}

		m_FrameIndex = (m_FrameIndex + 1) % (u32)m_Frames.size();
	}

	void SwapchainRenderTarget::recreate_swapchain(void)
	{
		const auto& logical_device = Device::Get()->logical_device();

		if (auto window = m_Window.lock())
		{
			m_Width = window->width();
			m_Height = window->height();
		}

		m_Viewport.y = (float)m_Height;
		m_Viewport.width = (float)m_Width;
		m_Viewport.height = -(float)m_Height;
		m_Scissor.width = m_Width;
		m_Scissor.height = m_Height;

		logical_device.waitIdle();

		for (auto& framebuffer : m_Framebuffers)
			logical_device.destroyFramebuffer(framebuffer);

		m_DepthImage.destroy();
		m_ColorImage.destroy();

		for (auto& img_view : m_ImageViews)
			logical_device.destroyImageView(img_view);

		logical_device.destroySwapchainKHR(m_Swapchain);

		this->_create_swapchain();
		this->_create_image_views();
		this->_create_color_buffer();
		this->_create_depth_buffer();
		this->_create_framebuffers();
	}

	void SwapchainRenderTarget::set_viewport(const Viewport& viewport)
	{
		m_Viewport = viewport;
		m_Viewport.y = (float)m_Height - viewport.y - viewport.height;
	}

	void SwapchainRenderTarget::set_scissor(const Scissor& scissor)
	{
		m_Scissor = scissor;
	}

	RenderTargetSubmitInfo SwapchainRenderTarget::submit_info(void) const
	{
		const FrameData& fd = m_Frames[m_FrameIndex];

		return RenderTargetSubmitInfo{
			.wait_semaphore = fd.image_available_semaphore,
			.wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput,
			.signal_semaphore = fd.render_finished_semaphore,
			.fence = fd.in_flight_fence
		};
	}

	void SwapchainRenderTarget::_create_window_surface(void)
	{
		if (auto window = m_Window.lock())
		{
			m_Surface = Internal::CreateWindowSurface(window->native());

			m_Width = window->width();
			m_Height = window->height();
		}

		m_Viewport.x = 0.0f;
		m_Viewport.y = (float)m_Height;
		m_Viewport.width = (float)m_Width;
		m_Viewport.height = -(float)m_Height;
		m_Viewport.min_depth = 0.0f;
		m_Viewport.max_depth = 1.0f;

		m_Scissor.x = 0;
		m_Scissor.y = 0;
		m_Scissor.width = m_Width;
		m_Scissor.height = m_Height;
	}

	void SwapchainRenderTarget::_create_swapchain(void)
	{
		const auto& logical_device = Device::Get()->logical_device();

		Internal::SurfaceSupport support(Device::Get()->physical_device(), m_Surface);
		NA_VERIFY(support, "Failed to create RendererWindow: Swapchain not supported!");

		m_Width = support.capabilities().currentExtent.width;
		m_Height = support.capabilities().currentExtent.height;
		m_SwapchainFormat = pickSurfaceFormat(support.formats());

		vk::SwapchainCreateInfoKHR create_info;
		create_info.surface = m_Surface;

		create_info.imageFormat = m_SwapchainFormat.format;
		create_info.imageColorSpace = m_SwapchainFormat.colorSpace;
		create_info.imageExtent = support.capabilities().currentExtent;

		create_info.imageArrayLayers = 1;
		create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		if (support.capabilities().maxImageCount > 0
			&& support.capabilities().minImageCount + 1 > support.capabilities().maxImageCount)
			create_info.minImageCount = support.capabilities().maxImageCount;
		else
			create_info.minImageCount = support.capabilities().minImageCount + 1;

		create_info.imageSharingMode = vk::SharingMode::eExclusive;

		create_info.preTransform = support.capabilities().currentTransform;
		create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		create_info.presentMode = pickPresentMode(support.present_modes());
		create_info.clipped = true;

		m_Swapchain = logical_device.createSwapchainKHR(create_info);

		u32 img_count;
		(void)logical_device.getSwapchainImagesKHR(m_Swapchain, &img_count, nullptr);
		m_Images.reallocate(img_count, img_count);
		(void)logical_device.getSwapchainImagesKHR(m_Swapchain, &img_count, m_Images.ptr());

		m_ImageInFlightFences.reallocate(img_count, img_count);
		for (u32 i = 0; i < img_count; i++)
			m_ImageInFlightFences[i] = nullptr;
	}

	void SwapchainRenderTarget::_create_image_views(void)
	{
		m_ImageViews.reallocate(m_Images.size(), m_Images.size());
		for (u64 i = 0; i < m_Images.size(); i++)
			m_ImageViews[i] = CreateImageView(
				m_Images[i],
				vk::ImageAspectFlagBits::eColor,
				m_SwapchainFormat.format,
				1 // layer count
			);
	}

	void SwapchainRenderTarget::_create_color_buffer(void)
	{
		bool msaa_enabled = m_RendererSettings->multisampling_enabled();

		if (!msaa_enabled)
			return;

		vk::SampleCountFlagBits sample_count = Device::Get()->vk_limits().vk_msaa_sample_count_if(msaa_enabled);

		DeviceImageCreateInfo2 create_info
		{
			.extent = vk::Extent3D(m_Width, m_Height, 1),
			.layer_count = 1,
			.aspect_mask = vk::ImageAspectFlagBits::eColor,
			.format = m_SwapchainFormat.format,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
			.sharing_mode = vk::SharingMode::eExclusive,
			.sample_count = sample_count,
			.memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal
		};
		m_ColorImage = DeviceImage(create_info);
	}

	void SwapchainRenderTarget::_create_depth_buffer(void)
	{
		vk::Format depth_format = FindSupportedFormat(
			{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);

		bool msaa_enabled = m_RendererSettings->multisampling_enabled();
		vk::SampleCountFlagBits sample_count = Device::Get()->vk_limits().vk_msaa_sample_count_if(msaa_enabled);

		DeviceImageCreateInfo2 create_info
		{
			.extent = vk::Extent3D(m_Width, m_Height, 1),
			.layer_count = 1,
			.aspect_mask = vk::ImageAspectFlagBits::eDepth,
			.format = depth_format,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
			.sharing_mode = vk::SharingMode::eExclusive,
			.sample_count = sample_count,
			.memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal
		};
		m_DepthImage = DeviceImage(create_info);
	}

	void SwapchainRenderTarget::_create_render_pass(void)
	{
		bool msaa_enabled = m_RendererSettings->multisampling_enabled();
		vk::SampleCountFlagBits sample_count = Device::Get()->vk_limits().vk_msaa_sample_count_if(msaa_enabled);

		std::array<vk::AttachmentDescription, 3> attachments{};
		attachments.fill({});

		vk::Format depth_format = VulkanImpl::FindSupportedFormat(
			{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);

		vk::AttachmentDescription& color_attachment = attachments[0];
		vk::AttachmentDescription& depth_attachment = attachments[1];
		vk::AttachmentDescription& color_attachment_resolve = attachments[2];

		vk::AttachmentReference color_attachment_ref;
		vk::AttachmentReference depth_attachment_ref;
		vk::AttachmentReference color_attachment_resolve_ref;

		vk::SubpassDescription subpass;
		vk::SubpassDependency dependency;

		color_attachment.format = m_SwapchainFormat.format;
		color_attachment.samples = sample_count;
		color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
		color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
		color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment.initialLayout = vk::ImageLayout::eUndefined;
		color_attachment.finalLayout = msaa_enabled ?
			vk::ImageLayout::eColorAttachmentOptimal :
			vk::ImageLayout::ePresentSrcKHR;

		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

		depth_attachment.format = depth_format;
		depth_attachment.samples = sample_count;
		depth_attachment.loadOp = vk::AttachmentLoadOp::eClear;
		depth_attachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		depth_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depth_attachment.initialLayout = vk::ImageLayout::eUndefined;
		depth_attachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		color_attachment_resolve.format = m_SwapchainFormat.format;
		color_attachment_resolve.samples = vk::SampleCountFlagBits::e1;
		color_attachment_resolve.loadOp = vk::AttachmentLoadOp::eDontCare;
		color_attachment_resolve.storeOp = vk::AttachmentStoreOp::eStore;
		color_attachment_resolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		color_attachment_resolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment_resolve.initialLayout = vk::ImageLayout::eUndefined;
		color_attachment_resolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		color_attachment_resolve_ref.attachment = msaa_enabled ? 2 : VK_ATTACHMENT_UNUSED;
		color_attachment_resolve_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;

		subpass.pResolveAttachments = &color_attachment_resolve_ref;

		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
			| vk::PipelineStageFlagBits::eLateFragmentTests;

		dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
			| vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
			| vk::PipelineStageFlagBits::eEarlyFragmentTests;

		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
			| vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		vk::RenderPassCreateInfo create_info;

		create_info.attachmentCount = (u32)attachments.size() - !msaa_enabled;
		create_info.pAttachments = attachments.data();

		create_info.subpassCount = 1;
		create_info.pSubpasses = &subpass;

		create_info.dependencyCount = 1;
		create_info.pDependencies = &dependency;

		m_RenderPass = Device::Get()->logical_device().createRenderPass(create_info);
	}

	void SwapchainRenderTarget::_create_framebuffers(void)
	{
		bool msaa_enabled = m_RendererSettings->multisampling_enabled();

		m_Framebuffers.reallocate(m_ImageViews.size(), m_ImageViews.size());
		for (u64 i = 0; i < m_ImageViews.size(); i++)
		{
			std::array<vk::ImageView, 3> attachments = {
				msaa_enabled ? m_ColorImage.img_view() : m_ImageViews[i],
				m_DepthImage.img_view(),
				msaa_enabled ? m_ImageViews[i] : nullptr,
			};

			vk::FramebufferCreateInfo create_info;

			create_info.renderPass = m_RenderPass;

			create_info.attachmentCount = (u32)attachments.size() - !msaa_enabled;
			create_info.pAttachments = attachments.data();

			create_info.width = m_Width;
			create_info.height = m_Height;
			create_info.layers = 1;

			m_Framebuffers[i] = Device::Get()->logical_device().createFramebuffer(create_info);
		}
	}

	void SwapchainRenderTarget::_create_sync_objects(void)
	{
		const auto& logical_device = Device::Get()->logical_device();

		vk::SemaphoreCreateInfo semaphore_info;

		vk::FenceCreateInfo fence_info;
		fence_info.flags = vk::FenceCreateFlagBits::eSignaled;

		for (u32 i = 0; i < m_Frames.size(); i++)
		{
			m_Frames[i].in_flight_fence = logical_device.createFence(fence_info);

			m_Frames[i].image_available_semaphore = logical_device.createSemaphore(semaphore_info);
			m_Frames[i].render_finished_semaphore = logical_device.createSemaphore(semaphore_info);
		}

	}
} // namespace Na::VulkanImpl
