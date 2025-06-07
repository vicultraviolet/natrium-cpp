#include "Pch.hpp"
#include "Natrium/Graphics/Renderer/Renderer.hpp"

#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

namespace Na {
	Renderer::Renderer(Window& window, AssetHandle<RendererSettings> settings)
	: m_Core(window, settings)
	{
		m_Frames.resize(m_Core.m_Settings->max_frames_in_flight());
		m_ImageInFlightFences.resize(m_Core.m_Images.size());

		this->_create_command_objects();
		this->_create_sync_objects();
	}

	void Renderer::destroy(void)
	{
		if (!VkContext::Exists())
			return;

		vk::Device logical_device = VkContext::GetLogicalDevice();

		for (FrameData& fd : m_Frames)
		{
			logical_device.destroyFence(fd.in_flight_fence);

			logical_device.destroySemaphore(fd.image_available_semaphore);
			logical_device.destroySemaphore(fd.render_finished_semaphore);
		}

		logical_device.destroyCommandPool(m_GraphicsCmdPool);

		m_Core.destroy();
	}

	bool Renderer::begin_frame(const glm::vec4& color)
	{
		//g_Logger.fmt(Na::Info, "Frame #{}, Image #{}", m_FrameIndex, m_ImageIndex);

		vk::Device logical_device = VkContext::GetLogicalDevice();
		FrameData& fd = m_Frames[m_FrameIndex];

		fd.valid = true;

		if (m_Core.m_Width  != m_Core.m_Window->width() ||
			m_Core.m_Height != m_Core.m_Window->height())
		{
			m_Core._recreate_swapchain();
			return fd.valid = false;
		}

		vk::Result result = vk::Result::eSuccess;

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
			m_Core.m_Swapchain,
			UINT64_MAX, // timeout
			fd.image_available_semaphore,
			nullptr,
			&m_ImageIndex
		);

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			m_Core._recreate_swapchain();
			return fd.valid = false;
		} else
		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			throw std::runtime_error(NA_FORMAT(
				"Failed to begin frame #{} with image #{}:"
				"Error in acquiring swapchain image!",
					m_FrameIndex,
					m_ImageIndex
			));

		if (m_ImageInFlightFences[m_ImageIndex])
		{
			result = logical_device.waitForFences({ m_ImageInFlightFences[m_ImageIndex] }, VK_TRUE, UINT64_MAX);
			NA_VERIFY_VK(result, "Failed to begin frame #{} with image #{}: Error in waiting for fence!", m_FrameIndex, m_ImageIndex);
		}
		m_ImageInFlightFences[m_ImageIndex] = fd.in_flight_fence;

		result = logical_device.resetFences(1, &fd.in_flight_fence);
		NA_VERIFY_VK(result, "Failed to begin frame #{} with image #{}: Error in resetting fence!", m_FrameIndex, m_ImageIndex);
		fd.cmd_buffer.reset();

		vk::CommandBufferBeginInfo begin_info;
		fd.cmd_buffer.begin(begin_info);

		std::array<vk::ClearValue, 2> clear_values;
		clear_values[0].color = std::array<float, 4>{ color.r, color.g, color.g, color.a };
		clear_values[1].depthStencil = { { 1.0f, 0 } };

		vk::RenderPassBeginInfo render_pass_info;

		render_pass_info.renderPass = m_Core.m_RenderPass;
		render_pass_info.framebuffer = m_Core.m_Framebuffers[m_ImageIndex];

		render_pass_info.renderArea.offset = { { 0, 0 } };
		render_pass_info.renderArea.extent = m_Core.m_Extent;

		render_pass_info.clearValueCount = (u32)clear_values.size();
		render_pass_info.pClearValues = clear_values.data();

		fd.cmd_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

		fd.cmd_buffer.setViewport(0, 1, &m_Core.m_Viewport);
		fd.cmd_buffer.setScissor(0, 1, &m_Core.m_Scissor);

		return true;
	}

	void Renderer::end_frame(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();
		FrameData& fd = m_Frames[m_FrameIndex];

		vk::Result result = vk::Result::eSuccess;

		fd.cmd_buffer.endRenderPass();
		fd.cmd_buffer.end();

		vk::SubmitInfo submit_info;

		vk::Semaphore wait_semaphores[] = { fd.image_available_semaphore };
		vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		submit_info.waitSemaphoreCount = sizeof(wait_semaphores) / sizeof(vk::Semaphore);
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;

		vk::Semaphore signal_semaphores[] = { fd.render_finished_semaphore };
		submit_info.signalSemaphoreCount = sizeof(signal_semaphores) / sizeof(VkSemaphore);
		submit_info.pSignalSemaphores = signal_semaphores;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &fd.cmd_buffer;

		result = VkContext::GetGraphicsQueue().submit(1, &submit_info, fd.in_flight_fence);
		NA_VERIFY_VK(
			result,
			"Failed to end frame #{} with image #{}:"
			"Error in submitting to graphics queue!",
				m_FrameIndex,
				m_ImageIndex
		);

		vk::PresentInfoKHR present_info;
		present_info.waitSemaphoreCount = submit_info.waitSemaphoreCount;
		present_info.pWaitSemaphores = signal_semaphores;

		present_info.swapchainCount = 1;
		present_info.pSwapchains = &m_Core.m_Swapchain;
		present_info.pImageIndices = &m_ImageIndex;

		try
		{
			result = VkContext::GetGraphicsQueue().presentKHR(present_info);
			switch (result)
			{
			case vk::Result::eSuboptimalKHR:
				m_Core._recreate_swapchain();
				break;
			case vk::Result::eErrorOutOfDateKHR:
				m_Core._recreate_swapchain();
				break;
			case vk::Result::eSuccess:
				break;
			default:
				NA_VERIFY_VK(result, "Failed to end frame #{} with image #{}: Error in presenting to graphics queue!", m_FrameIndex, m_ImageIndex);
			}

		} catch (const vk::OutOfDateKHRError& err)
		{
			(void)err;
			m_Core._recreate_swapchain();
		}

		m_FrameIndex = (m_FrameIndex + 1) % (u32)m_Frames.size();
	}

	void Renderer::bind_pipeline(const GraphicsPipeline& pipeline)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();
		FrameData& fd = m_Frames[m_FrameIndex];

		fd.cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline());

		if (pipeline.descriptor_set())
			fd.cmd_buffer.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				pipeline.layout(),
				0, // first set
				1, &pipeline.descriptor_set(),
				pipeline.dynamic_offset_count(), pipeline.dynamic_offsets().ptr() + (m_FrameIndex * pipeline.dynamic_offset_count())
			);
	}

	void Renderer::set_push_constant(
		const PushConstant& push_constant,
		const void* data,
		const GraphicsPipeline& pipeline
	)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();
		FrameData& fd = m_Frames[m_FrameIndex];

		fd.cmd_buffer.pushConstants(
			pipeline.layout(),
			(vk::ShaderStageFlagBits)push_constant.shader_stage,
			push_constant.offset,
			push_constant.size,
			data
		);
	}

	void Renderer::draw_vertices(const VertexBuffer& vertex_buffer, u32 vertex_count, u32 instance_count)
	{
		FrameData& fd = m_Frames[m_FrameIndex];

		fd.cmd_buffer.bindVertexBuffers(0, { vertex_buffer.native() }, { 0 });

		fd.cmd_buffer.draw(
			vertex_count,
			instance_count,
			0, // first vertex
			0 // first instance
		);
	}

	void Renderer::draw_indexed(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer, u32 instance_count)
	{
		FrameData& fd = m_Frames[m_FrameIndex];

		fd.cmd_buffer.bindVertexBuffers(0, { vertex_buffer.native() }, { 0 });
		fd.cmd_buffer.bindIndexBuffer(index_buffer.native(), 0, vk::IndexType::eUint32);

		fd.cmd_buffer.drawIndexed(
			index_buffer.count(),
			instance_count,
			0,  // first index
			0, // vertex offset
			0 // first instance
		);
	}

	void Renderer::set_descriptor_buffer(void* buffer, const void* data) const
	{
		NA_ASSERT(buffer, "Failed to set descriptor buffer: buffer is null!");
		NA_ASSERT(data, "Failed to set descriptor buffer: data is null!");

		ShaderUniformType uniform_type = *(const ShaderUniformType*)buffer;
		switch (uniform_type)
		{
			case ShaderUniformType::UniformBuffer:
			{
				UniformBuffer& uniform_buffer = *(UniformBuffer*)buffer;
				void* mapped = (Byte*)(uniform_buffer.mapped_data()) + (m_FrameIndex * uniform_buffer.aligned_size());
				memcpy(mapped, data, uniform_buffer.per_frame_size());
				break;
			}
			case ShaderUniformType::StorageBuffer:
			{
				StorageBuffer& storage_buffer = *(StorageBuffer*)buffer;
				void* mapped = (Byte*)(storage_buffer.mapped_data()) + (m_FrameIndex * storage_buffer.aligned_size());
				memcpy(mapped, data, storage_buffer.per_frame_size());
				break;
			}
			default:
			{
				throw std::runtime_error("Failed to set descriptor buffer: buffer has unknown type!");
				break;
			}
		}
	}

	void Renderer::_create_command_objects(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		vk::CommandPoolCreateInfo graphics_pool_info;
		graphics_pool_info.queueFamilyIndex = m_Core.m_QueueIndices.graphics;
		graphics_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		m_GraphicsCmdPool = logical_device.createCommandPool(graphics_pool_info);

		vk::CommandBufferAllocateInfo cmd_alloc_info;
		cmd_alloc_info.commandPool = m_GraphicsCmdPool;
		cmd_alloc_info.level = vk::CommandBufferLevel::ePrimary;
		cmd_alloc_info.commandBufferCount = (u32)m_Frames.size();

		for (u64 i = 0; auto cmd_buffer : logical_device.allocateCommandBuffers(cmd_alloc_info))
			m_Frames[i++].cmd_buffer = cmd_buffer;
	}

	void Renderer::_create_sync_objects(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

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

	Renderer::Renderer(Renderer&& other)
	: m_Core(std::move(other.m_Core)),
	m_GraphicsCmdPool(std::exchange(other.m_GraphicsCmdPool, nullptr)),
	m_Frames(std::move(other.m_Frames)),
	m_FrameIndex(other.m_FrameIndex),
	m_ImageInFlightFences(std::move(other.m_ImageInFlightFences)),
	m_ImageIndex(other.m_ImageIndex)
	{}

	Renderer& Renderer::operator=(Renderer&& other)
	{
		this->destroy();

		m_Core = std::move(other.m_Core);
		m_GraphicsCmdPool = std::exchange(other.m_GraphicsCmdPool, nullptr);
		m_Frames = std::move(other.m_Frames);
		m_FrameIndex = other.m_FrameIndex;
		m_ImageInFlightFences = std::move(other.m_ImageInFlightFences);
		m_ImageIndex = other.m_ImageIndex;

		return *this;
	}
} // namespace Na
