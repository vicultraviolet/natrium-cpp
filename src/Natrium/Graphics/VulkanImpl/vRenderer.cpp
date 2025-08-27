#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"

#include "Internal.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"
#include "Natrium/Graphics/VulkanImpl/vTrianglePipeline.hpp"
#include "Natrium/Graphics/VulkanImpl/vComputePipeline.hpp"

#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"

#include "Natrium/Graphics/VulkanImpl/vUniformSet.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

#include <imgui/backends/imgui_impl_vulkan.h>

namespace Na::VulkanImpl {
	Renderer::Renderer(Ref<const RendererSettingsAsset> settings)
	: m_RendererSettings(settings),
	  m_CommandBuffers(settings->max_frames_in_flight(), settings->max_frames_in_flight())
	{
		this->_create_cmd_objects();
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
		
		if (m_GraphicsCommandPool)
		{
			logical_device.destroyCommandPool(m_GraphicsCommandPool);
			m_GraphicsCommandPool = nullptr;
		}
	}

	void Renderer::begin_frame(void)
	{
		const auto& logical_device = Device::Get()->logical_device();
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		cmd_buffer.reset();

		vk::CommandBufferBeginInfo begin_info;
		cmd_buffer.begin(begin_info);
	}

	void Renderer::end_frame(void)
	{
		const auto& logical_device = Device::Get()->logical_device();
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		vk::Result result = vk::Result::eSuccess;

		cmd_buffer.end();

		RenderTargetSubmitInfo render_target_submit_info;
		if (auto render_target = m_RenderTarget.lock())
		{
			switch (render_target->type())
			{
			case RenderTargetType::Swapchain:
				render_target_submit_info = static_ref_cast<const SwapchainRenderTarget>(render_target)->submit_info();
				break;
			default:
				throw std::runtime_error("Unsupported render target type for end_frame!");
			}
		}

		vk::SubmitInfo submit_info;

		submit_info.setWaitSemaphores({ render_target_submit_info.wait_semaphore });
		submit_info.setWaitDstStageMask({ render_target_submit_info.wait_stage });
		submit_info.setSignalSemaphores({ render_target_submit_info.signal_semaphore });
		submit_info.setCommandBuffers({ m_CommandBuffers[m_FrameIndex] });

		result = Device::Get()->graphics_queue().submit(1, &submit_info, render_target_submit_info.fence);

		NA_VERIFY_VK(
			result,
			"Failed to end frame #{}:"
			"Error in submitting to graphics queue!",
				m_FrameIndex
		);

		m_FrameIndex = (m_FrameIndex + 1) % (u32)m_CommandBuffers.size();
	}

	void Renderer::begin_render_pass(const glm::vec4& color)
	{
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		if (auto _render_target = m_RenderTarget.lock())
		{
			switch (_render_target->type())
			{
			case RenderTargetType::Swapchain:
				static_ref_cast<const SwapchainRenderTarget>(_render_target)->begin_render_pass(cmd_buffer, color);
				break;
			default:
				throw std::runtime_error("Failed to begin render pass: Unknown render target type!");
			}
		}
		else
		{
			g_Logger.print(Warn, "Failed to begin render pass: No render target bound!");
		}
	}

	void Renderer::end_render_pass(void)
	{
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		cmd_buffer.endRenderPass();
	}

	void Renderer::draw_imgui(void)
	{
#if !defined(NA_DISABLE_IMGUI)
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);
#endif // NA_DISABLE_IMGUI
	}

	void Renderer::bind_pipeline(View<const Graphics::Pipeline> _pipeline)
	{
		const auto& logical_device = Device::Get()->logical_device();
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		auto pipeline = (const Pipeline*)(((Byte*)_pipeline.ptr()) + k_PipelineOffset);

		cmd_buffer.bindPipeline(PipelineTypeToVk(_pipeline->type()), pipeline->pipeline);
	}

	void Renderer::bind_uniform_set(
		View<const Graphics::UniformSet> _uniform_set,
		View<const Graphics::Pipeline> _pipeline,
		u32 set_index
	)
	{
		const auto& logical_device = Device::Get()->logical_device();
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		auto uniform_set = static_ref_cast<const UniformSet>(_uniform_set);

		u64 stride = (u64)m_FrameIndex * (u64)uniform_set->dynamic_offset_count();
		const u32* dynamic_offsets = uniform_set->dynamic_offsets().ptr() + stride;

		auto pipeline = (const Pipeline*)(((Byte*)_pipeline.ptr()) + k_PipelineOffset);

		cmd_buffer.bindDescriptorSets(
			PipelineTypeToVk(_pipeline->type()),
			pipeline->layout,
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
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

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

		auto pipeline = (const Pipeline*)(((Byte*)_pipeline.ptr()) + k_PipelineOffset);

		cmd_buffer.bindDescriptorSets(
			PipelineTypeToVk(_pipeline->type()),
			pipeline->layout,
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
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		auto pipeline = (const Pipeline*)(((Byte*)_pipeline.ptr()) + k_PipelineOffset);

		cmd_buffer.pushConstants(
			pipeline->layout,
			ShaderStageToVk(stage),
			offset,
			size,
			data
		);
	}

	void Renderer::bind_vertex_buffer(
		View<const Graphics::Buffer> _vertex_buffer,
		u64 offset
	)
	{
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		auto vertex_buffer = static_ref_cast<const Buffer>(_vertex_buffer);

		cmd_buffer.bindVertexBuffers(0, { vertex_buffer->native() }, { offset });
	}

	void Renderer::bind_index_buffer(
		View<const Graphics::Buffer> _index_buffer,
		u64 offset
	)
	{
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		auto index_buffer = static_ref_cast<const Buffer>(_index_buffer);

		cmd_buffer.bindIndexBuffer(index_buffer->native(), offset, vk::IndexType::eUint32);
	}

	void Renderer::draw_vertices(
		u32 vertex_count,
		u32 instance_count,
		u32 first_vertex,
		u32 first_instance
	)
	{
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		cmd_buffer.draw(
			vertex_count,
			instance_count,
			first_vertex,
			first_instance
		);
	}

	void Renderer::draw_indexed(
		u32 index_count,
		u32 instance_count,
		u32 first_index,
		u32 first_instance
	)
	{
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		cmd_buffer.drawIndexed(
			index_count,
			instance_count,
			first_index,
			0, // vertex offset
			first_instance
		);
	}

	void Renderer::dispatch_compute(glm::uvec3 workgroup_count)
	{
		vk::CommandBuffer cmd_buffer = m_CommandBuffers[m_FrameIndex];

		cmd_buffer.dispatch(workgroup_count.x, workgroup_count.y, workgroup_count.z);
	}

	void Renderer::_create_cmd_objects(void)
	{
		const auto& logical_device = Device::Get()->logical_device();

		vk::CommandPoolCreateInfo graphics_pool_info;
		graphics_pool_info.queueFamilyIndex = Device::Get()->graphics_queue_index();
		graphics_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		m_GraphicsCommandPool = logical_device.createCommandPool(graphics_pool_info);

		vk::CommandBufferAllocateInfo cmd_alloc_info;
		cmd_alloc_info.commandPool = m_GraphicsCommandPool;
		cmd_alloc_info.level = vk::CommandBufferLevel::ePrimary;
		cmd_alloc_info.commandBufferCount = (u32)m_CommandBuffers.size();

		for (u64 i = 0; const auto& cmd_buffer : logical_device.allocateCommandBuffers(cmd_alloc_info))
			m_CommandBuffers[i++] = cmd_buffer;
	}

	void Renderer::_create_descriptor_pool(void)
	{
		constexpr u32 k_Count = 5;

		constexpr u32 k_MaxUniformBuffers = 8;
		constexpr u32 k_MaxStorageBuffers = 8;
		constexpr u32 k_MaxMultiUniformBuffers = 8;
		constexpr u32 k_MaxMultiStorageBuffers = 8;
		constexpr u32 k_MaxTextures = 32;

		constexpr u32 k_MaxSets = k_MaxUniformBuffers + k_MaxStorageBuffers + k_MaxMultiUniformBuffers + k_MaxMultiStorageBuffers + k_MaxTextures;

		std::array<vk::DescriptorPoolSize, k_Count> descriptor_pool_sizes{};

		descriptor_pool_sizes[0].type = vk::DescriptorType::eUniformBufferDynamic;
		descriptor_pool_sizes[0].descriptorCount = k_MaxUniformBuffers;

		descriptor_pool_sizes[1].type = vk::DescriptorType::eStorageBufferDynamic;
		descriptor_pool_sizes[1].descriptorCount = k_MaxStorageBuffers;

		descriptor_pool_sizes[2].type = vk::DescriptorType::eUniformBuffer;
		descriptor_pool_sizes[2].descriptorCount = k_MaxUniformBuffers;

		descriptor_pool_sizes[3].type = vk::DescriptorType::eStorageBuffer;
		descriptor_pool_sizes[3].descriptorCount = k_MaxStorageBuffers;

		descriptor_pool_sizes[4].type = vk::DescriptorType::eCombinedImageSampler;
		descriptor_pool_sizes[4].descriptorCount = k_MaxTextures;

		vk::DescriptorPoolCreateInfo create_info;

		create_info.maxSets = k_MaxSets;
		create_info.poolSizeCount = k_Count;
		create_info.pPoolSizes = descriptor_pool_sizes.data();

		m_DescriptorPool = Device::Get()->logical_device().createDescriptorPool(create_info);
	}
} // namespace Na
