#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"

#include "Internal.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"
#include "Natrium/Graphics/VulkanImpl/vTrianglePipeline.hpp"

#include "Natrium/Graphics/VulkanImpl/vVertexBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vIndexBuffer.hpp"

#include "Natrium/Graphics/VulkanImpl/vUniformBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vStorageBuffer.hpp"

#include "Natrium/Graphics/VulkanImpl/vUniformSet.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

namespace Na::VulkanImpl {
	Renderer::Renderer(const Window& window, Ref<const RendererSettingsAsset> settings)
	: m_Window(window, settings),
	m_Frames(m_Window.settings()->max_frames_in_flight()),
	m_ImageInFlightFences(m_Window.images().size())
	{
		m_Frames.resize(m_Frames.capacity());
		m_ImageInFlightFences.resize(m_ImageInFlightFences.capacity());
		std::memset(m_ImageInFlightFences.ptr(), 0, m_Window.images().size() * sizeof(m_ImageInFlightFences[0]));

		this->_create_command_objects();
		this->_create_sync_objects();
		this->_create_descriptor_pool();
	}

	void Renderer::destroy(void)
	{
		if (!Device::Get())
			return;

		const auto& logical_device = Device::Get()->logical_device();

		if (m_DescriptorPool)
		{
			logical_device.destroyDescriptorPool(m_DescriptorPool);
			m_DescriptorPool = nullptr;
		}

		for (FrameData& fd : m_Frames)
		{
			logical_device.destroyFence(fd.in_flight_fence);

			logical_device.destroySemaphore(fd.image_available_semaphore);
			logical_device.destroySemaphore(fd.render_finished_semaphore);
		}
		m_Frames.destroy();

		if (m_GraphicsCmdPool)
		{
			logical_device.destroyCommandPool(m_GraphicsCmdPool);
			m_GraphicsCmdPool = nullptr;
		}

		m_Window.destroy();
	}

	bool Renderer::begin_frame(const glm::vec4& color)
	{
		//g_Logger.fmt(Na::Info, "Frame #{}, Image #{}", m_FrameIndex, m_ImageIndex);

		const auto& logical_device = Device::Get()->logical_device();
		FrameData& fd = m_Frames[m_FrameIndex];

		fd.valid = true;

		if (m_Window.width()  != m_Window.window().width() ||
			m_Window.height() != m_Window.window().height())
		{
			m_Window.recreate_swapchain();
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
			m_Window.swapchain(),
			UINT64_MAX, // timeout
			fd.image_available_semaphore,
			nullptr,
			&m_ImageIndex
		);

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			m_Window.recreate_swapchain();
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
		clear_values[0].color = std::array<float, 4>{ color.r, color.g, color.b, color.a };
		clear_values[1].depthStencil = { { 1.0f, 0 } };

		vk::RenderPassBeginInfo render_pass_info;

		render_pass_info.renderPass = m_Window.render_pass();
		render_pass_info.framebuffer = m_Window.framebuffers()[m_ImageIndex];

		render_pass_info.renderArea.offset = { { 0, 0 } };
		render_pass_info.renderArea.extent = vk::Extent2D(m_Window.width(), m_Window.height());

		render_pass_info.clearValueCount = (u32)clear_values.size();
		render_pass_info.pClearValues = clear_values.data();

		fd.cmd_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

		fd.cmd_buffer.setViewport(0, 1, &m_Window.viewport());
		fd.cmd_buffer.setScissor(0, 1, &m_Window.scissor());

		return true;
	}

	void Renderer::end_frame(void)
	{
		const auto& logical_device = Device::Get()->logical_device();
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

		result = Device::Get()->graphics_queue().submit(1, &submit_info, fd.in_flight_fence);

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
		present_info.pSwapchains = &m_Window.swapchain();
		present_info.pImageIndices = &m_ImageIndex;

		try
		{
			result = Device::Get()->graphics_queue().presentKHR(present_info);
			switch (result)
			{
			case vk::Result::eSuboptimalKHR:
				m_Window.recreate_swapchain();
				break;
			case vk::Result::eErrorOutOfDateKHR:
				m_Window.recreate_swapchain();
				break;
			case vk::Result::eSuccess:
				break;
			default:
				NA_VERIFY_VK(result, "Failed to end frame #{} with image #{}: Error in presenting to graphics queue!", m_FrameIndex, m_ImageIndex);
			}

		} catch (const vk::OutOfDateKHRError& err)
		{
			(void)err;
			m_Window.recreate_swapchain();
		}

		m_FrameIndex = (m_FrameIndex + 1) % (u32)m_Frames.size();
	}

	void Renderer::bind_pipeline(View<const Graphics::Pipeline> _pipeline)
	{
		const auto& logical_device = Device::Get()->logical_device();
		FrameData& fd = m_Frames[m_FrameIndex];

		auto pipeline = static_ref_cast<const TrianglePipeline>(_pipeline);

		fd.cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->pipeline());
	}

	void Renderer::bind_uniform_set(
		View<const Graphics::UniformSet> _uniform_set,
		View<const Graphics::Pipeline> _pipeline,
		u32 set_index
	)
	{
		const auto& logical_device = Device::Get()->logical_device();
		FrameData& fd = m_Frames[m_FrameIndex];

		auto pipeline = static_ref_cast<const TrianglePipeline>(_pipeline);
		auto uniform_set = static_ref_cast<const UniformSet>(_uniform_set);

		u64 stride = (u64)m_FrameIndex * (u64)uniform_set->dynamic_offset_count();
		const u32* dynamic_offsets = uniform_set->dynamic_offsets().ptr() + stride;

		fd.cmd_buffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			pipeline->layout(),
			set_index,
			1, &uniform_set->descriptor_set(),
			uniform_set->dynamic_offset_count(), dynamic_offsets
		);
	}

	void Renderer::bind_uniform_sets(
		const View<const Graphics::UniformSet>* uniform_sets,
		u64 set_count,
		View<const Graphics::Pipeline> _pipeline,
		u32 starting_index
	)
	{
		const auto& logical_device = Device::Get()->logical_device();
		FrameData& fd = m_Frames[m_FrameIndex];

		auto pipeline = static_ref_cast<const TrianglePipeline>(_pipeline);

		u64 dynamic_offset_index = 0;
		u64 dynamic_offset_count = 0;

		for (u64 i = 0; i < set_count; i++)
		{
			auto uniform_set = static_ref_cast<const UniformSet>(uniform_sets[i]);

			m_DescriptorSets[i] = uniform_set->descriptor_set();

			if (uniform_set->dynamic_offsets().empty())
				continue;

			dynamic_offset_count += uniform_set->dynamic_offset_count();

			memcpy(
				m_DynamicOffsets.data() + dynamic_offset_index,
				uniform_set->dynamic_offsets().ptr() + (u64)m_FrameIndex * (u64)uniform_set->dynamic_offset_count(),
				uniform_set->dynamic_offset_count() * sizeof(u32)
			);

			dynamic_offset_index += uniform_set->dynamic_offset_count() * sizeof(u32);
		}

		fd.cmd_buffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			pipeline->layout(),
			starting_index,
			(u32)set_count, m_DescriptorSets.data(),
			(u32)dynamic_offset_count, m_DynamicOffsets.data()
		);
	}

	void Renderer::set_push_constant(
		u32 size,
		ShaderStage stage,
		u32 offset,
		const void* data,
		View<const Graphics::Pipeline> _pipeline
	) const
	{
		const auto& logical_device = Device::Get()->logical_device();
		const FrameData& fd = m_Frames[m_FrameIndex];

		auto pipeline = static_ref_cast<const TrianglePipeline>(_pipeline);

		fd.cmd_buffer.pushConstants(
			pipeline->layout(),
			ShaderStageToVk(stage),
			offset,
			size,
			data
		);
	}

	void Renderer::draw_vertices(
		View<const Graphics::VertexBuffer> _vertex_buffer,
		u32 vertex_count,
		u32 instance_count,
		u32 first_vertex,
		u32 first_instance
	)
	{
		FrameData& fd = m_Frames[m_FrameIndex];

		auto vertex_buffer = static_ref_cast<const VertexBuffer>(_vertex_buffer);

		fd.cmd_buffer.bindVertexBuffers(0, { vertex_buffer->native() }, { 0 });

		fd.cmd_buffer.draw(
			vertex_count,
			instance_count,
			first_vertex,
			first_instance
		);
	}

	void Renderer::draw_indexed(
		View<const Graphics::VertexBuffer> _vertex_buffer,
		View<const Graphics::IndexBuffer> _index_buffer,
		u32 instance_count,
		u32 first_index,
		u32 first_instance
	)
	{
		FrameData& fd = m_Frames[m_FrameIndex];

		auto vertex_buffer = static_ref_cast<const VertexBuffer>(_vertex_buffer);
		auto index_buffer = static_ref_cast<const IndexBuffer>(_index_buffer);

		fd.cmd_buffer.bindVertexBuffers(0, { vertex_buffer->native() }, { 0 });
		fd.cmd_buffer.bindIndexBuffer(index_buffer->native(), 0, vk::IndexType::eUint32);

		fd.cmd_buffer.drawIndexed(
			index_buffer->count(),
			instance_count,
			first_index,
			0, // vertex offset
			first_instance
		);
	}

	void Renderer::set_descriptor_buffer(View<const Graphics::Uniform> buffer, const void* data) const
	{
		NA_ASSERT(buffer, "Failed to set descriptor buffer: buffer is null!");
		NA_ASSERT(data, "Failed to set descriptor buffer: data is null!");

		Graphics::UniformType type = buffer->type();
		switch (type)
		{
			case Graphics::UniformType::UniformBuffer:
			{
				auto ubo = static_ref_cast<const UniformBuffer>(buffer);

				void* mapped = (Byte*)(ubo->mapped_data()) + (m_FrameIndex * ubo->aligned_size());
				memcpy(mapped, data, ubo->per_frame_size());

				break;
			}
			case Graphics::UniformType::StorageBuffer:
			{
				auto ssbo = static_ref_cast<const StorageBuffer>(buffer);

				void* mapped = (Byte*)(ssbo->mapped_data()) + (m_FrameIndex * ssbo->aligned_size());
				memcpy(mapped, data, ssbo->per_frame_size());

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
		const auto& logical_device = Device::Get()->logical_device();

		vk::CommandPoolCreateInfo graphics_pool_info;
		graphics_pool_info.queueFamilyIndex = Device::Get()->graphics_queue_index();
		graphics_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		m_GraphicsCmdPool = logical_device.createCommandPool(graphics_pool_info);

		vk::CommandBufferAllocateInfo cmd_alloc_info;
		cmd_alloc_info.commandPool = m_GraphicsCmdPool;
		cmd_alloc_info.level = vk::CommandBufferLevel::ePrimary;
		cmd_alloc_info.commandBufferCount = (u32)m_Frames.size();

		for (u64 i = 0; const auto& cmd_buffer : logical_device.allocateCommandBuffers(cmd_alloc_info))
			m_Frames[i++].cmd_buffer = cmd_buffer;
	}

	void Renderer::_create_sync_objects(void)
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

	void Renderer::_create_descriptor_pool(void)
	{
		constexpr u32 k_Count = 3;

		constexpr u32 k_MaxUniformBuffers = 16;
		constexpr u32 k_MaxStorageBuffers = 16;
		constexpr u32 k_MaxTextures = 32;

		std::array<vk::DescriptorPoolSize, k_Count> descriptor_pool_sizes{};

		descriptor_pool_sizes[0].type = vk::DescriptorType::eUniformBufferDynamic;
		descriptor_pool_sizes[0].descriptorCount = k_MaxUniformBuffers;

		descriptor_pool_sizes[1].type = vk::DescriptorType::eStorageBufferDynamic;
		descriptor_pool_sizes[1].descriptorCount = k_MaxStorageBuffers;

		descriptor_pool_sizes[2].type = vk::DescriptorType::eCombinedImageSampler;
		descriptor_pool_sizes[2].descriptorCount = k_MaxTextures;

		vk::DescriptorPoolCreateInfo create_info;

		create_info.maxSets = k_MaxUniformBuffers + k_MaxStorageBuffers + k_MaxTextures;
		create_info.poolSizeCount = k_Count;
		create_info.pPoolSizes = descriptor_pool_sizes.data();

		m_DescriptorPool = Device::Get()->logical_device().createDescriptorPool(create_info);
	}

	Renderer::Renderer(Renderer&& other) noexcept
	: m_Window(std::move(other.m_Window)),

	m_GraphicsCmdPool(std::exchange(other.m_GraphicsCmdPool, nullptr)),

	m_Frames(std::move(other.m_Frames)),
	m_FrameIndex(other.m_FrameIndex),

	m_ImageInFlightFences(std::move(other.m_ImageInFlightFences)),
	m_ImageIndex(other.m_ImageIndex),

	m_DescriptorPool(std::exchange(other.m_DescriptorPool, nullptr))
	{}

	Renderer& Renderer::operator=(Renderer&& other) noexcept
	{
		this->destroy();

		m_Window = std::move(other.m_Window);

		m_GraphicsCmdPool = std::exchange(other.m_GraphicsCmdPool, nullptr);

		m_Frames = std::move(other.m_Frames);
		m_FrameIndex = other.m_FrameIndex;

		m_ImageInFlightFences = std::move(other.m_ImageInFlightFences);
		m_ImageIndex = other.m_ImageIndex;
		
		m_DescriptorPool = std::exchange(other.m_DescriptorPool, nullptr);

		return *this;
	}
} // namespace Na
