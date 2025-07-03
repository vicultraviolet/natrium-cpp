#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vPipeline.hpp"

#include "./vPipelineStates.hpp"

#include "Internal.hpp"
#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vStorageBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vTexture.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"

namespace Na::VulkanImpl {
	struct VertexInputInfo {
		vk::VertexInputBindingDescription binding;
		Na::ArrayList<vk::VertexInputAttributeDescription> attributes;

		vk::PipelineVertexInputStateCreateInfo pipeline_info;

		VertexInputInfo(void) = default;
		VertexInputInfo(const Graphics::VertexAttributes& vertex_attributes)
		{
			u64 attribute_count = vertex_attributes.count();
			attributes.reallocate(attribute_count, attribute_count);

			u32 offset = 0;
			for (u64 i = 0; const Graphics::VertexAttribute& attribute : vertex_attributes.attributes())
			{
				attributes[i].binding = attribute.binding;
				attributes[i].location = (u32)i;
				attributes[i].format = VertexAttributeTypeToVk(attribute.type);
				attributes[i].offset = offset; 

				offset += (u32)SizeOfVertexAttribute(attribute.type);
				i++;
			}

			binding.binding = 0;
			binding.stride = offset;
			binding.inputRate = vk::VertexInputRate::eVertex;

			pipeline_info.vertexBindingDescriptionCount = 1;
			pipeline_info.pVertexBindingDescriptions = &binding;
			pipeline_info.vertexAttributeDescriptionCount = (u32)attributes.size();
			pipeline_info.pVertexAttributeDescriptions = attributes.ptr();
		}
	};

	static vk::DescriptorSetLayout createDescriptorSetLayout(const Shaders& shaders)
	{
		u64 binding_count = 0;
		for (View<const Graphics::Shader> shader : shaders)
			binding_count += shader->uniform_count();

		Na::ArrayList<vk::DescriptorSetLayoutBinding> bindings(binding_count, binding_count);
		for (u64 i = 0; View<const Graphics::Shader> shader : shaders)
		{
			for (const auto& [binding, uniform] : shader->uniforms())
			{
				if (uniform->type() == Graphics::UniformType::None)
					continue;

				bindings[i].binding = binding;
				bindings[i].descriptorType = UniformTypeToVk(uniform->type());
				bindings[i].descriptorCount = 1; // uniform.count
				bindings[i].stageFlags = ShaderStageToVk(shader->stage());
				bindings[i].pImmutableSamplers = nullptr; // no samplers for now
				i++;
			}
		}

		vk::DescriptorSetLayoutCreateInfo create_info;
		create_info.bindingCount = (u32)binding_count;
		create_info.pBindings = bindings.ptr();

		return Internal::g_DeviceData.logical_device.createDescriptorSetLayout(create_info);
	}

	static vk::DescriptorPool createDescriptorPool(const Shaders& shaders)
	{
		u64 binding_count = 0;
		for (const View<const Graphics::Shader>& shader : shaders)
			binding_count += shader->uniform_count();

		Na::ArrayList<vk::DescriptorPoolSize> pool_sizes(binding_count, binding_count);
		for (u64 i = 0; View<const Graphics::Shader> shader : shaders)
		{
			for (const auto& [binding, uniform] : shader->uniforms())
			{
				if (uniform->type() == Graphics::UniformType::None)
					continue;

				pool_sizes[i].type = UniformTypeToVk(uniform->type());
				pool_sizes[i].descriptorCount = 1; // uniform.count
				i++;
			}
		}

		vk::DescriptorPoolCreateInfo create_info;
		create_info.poolSizeCount = (u32)pool_sizes.size();
		create_info.pPoolSizes = pool_sizes.ptr();
		create_info.maxSets = 1;

		return Internal::g_DeviceData.logical_device.createDescriptorPool(create_info);
	}

	static vk::DescriptorSet createDescriptorSet(vk::DescriptorSetLayout layout, vk::DescriptorPool pool)
	{
		vk::DescriptorSetAllocateInfo alloc_info;
		alloc_info.descriptorPool = pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &layout;

		vk::DescriptorSet descriptor_set;

		vk::Result result = Internal::g_DeviceData.logical_device.allocateDescriptorSets(&alloc_info, &descriptor_set);
		NA_VERIFY_VK(result, "Failed to allocate descriptor set!");

		return descriptor_set;
	}

	static Na::ArrayList<vk::DescriptorSet> createDescriptorSets(u32 count, vk::DescriptorSetLayout* layouts, vk::DescriptorPool pool)
	{
		vk::DescriptorSetAllocateInfo alloc_info;
		alloc_info.descriptorPool = pool;
		alloc_info.descriptorSetCount = count;
		alloc_info.pSetLayouts = layouts;

		Na::ArrayList<vk::DescriptorSet> descriptor_sets((u64)count, (u64)count);

		vk::Result result = Internal::g_DeviceData.logical_device.allocateDescriptorSets(&alloc_info, descriptor_sets.ptr());
		NA_VERIFY_VK(result, "Failed to allocate descriptor sets!");

		return descriptor_sets;
	}

	Pipeline::Pipeline(
		View<const Graphics::Renderer> _renderer,
		const Graphics::VertexAttributes& vertex_shader_layout,
		const Shaders& shaders
	)
	{
		auto renderer = static_ref_cast<const VulkanImpl::Renderer>(_renderer);

		const Na::ArrayList<vk::DynamicState> dynamic_states = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		VertexInputInfo vertex_input_info(vertex_shader_layout);

		auto dynamic_state_info = dynamicStateInfo(dynamic_states);
		auto viewport_info = viewportInfo();
		auto input_assembly_info = inputAssemblyInfo();
		auto rasterization_info = rasterizationInfo();
		auto multisample_info = multisampleInfo(renderer->settings()->multisampling_enabled());
		auto color_blend_attachment = colorBlendAttachment(false);
		auto color_blend_info = colorBlendInfo(color_blend_attachment);
		auto depth_stencil_info = depthStencilInfo();

		ArrayList<vk::PipelineShaderStageCreateInfo> shader_infos(shaders.size(), shaders.size());
		for (u64 i = 0; View<const Graphics::Shader> shader : shaders)
		{
			auto vk_shader = static_ref_cast<const VulkanImpl::Shader>(shader);

			shader_infos[i].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
			shader_infos[i].stage = ShaderStageToVk(shader->stage());
			shader_infos[i].module = vk_shader->module();
			shader_infos[i].pName = shader->entry_point().data();
			shader_infos[i].pSpecializationInfo = nullptr; 
			shader_infos[i].pNext = nullptr;
			shader_infos[i].flags = vk::PipelineShaderStageCreateFlags();

			i++;
		}

		bool has_uniforms = false;
		for (View<const Graphics::Shader> shader : shaders)
		{
			if (shader->has_uniforms())
			{
				has_uniforms = true;
				break;
			}
		}

		if (has_uniforms)
		{
			m_DescriptorLayout = createDescriptorSetLayout(shaders);
			m_DescriptorPool = createDescriptorPool(shaders);
			m_DescriptorSet = createDescriptorSet(m_DescriptorLayout, m_DescriptorPool);
			this->_set_descriptor_set(shaders, renderer->settings()->max_frames_in_flight());
		}
		else
		{
			m_DescriptorLayout = nullptr;
			m_DescriptorPool = nullptr;
			m_DescriptorSet = nullptr;
		}

		u64 push_constant_count = 0;
		for (View<const Graphics::Shader> shader : shaders)
		{
			if (shader->has_push_constant())
				push_constant_count++;
		}

		Na::ArrayList<vk::PushConstantRange> push_constant_ranges(push_constant_count, push_constant_count);
		for (u64 i = 0; View<const Graphics::Shader> shader : shaders)
		{
			if (!shader->has_push_constant())
				continue;

			push_constant_ranges[i].stageFlags = ShaderStageToVk(shader->stage());
			push_constant_ranges[i].offset = 0; 
			push_constant_ranges[i].size = shader->push_constant_size();
			
			i++;
		}

		m_Layout = Internal::g_DeviceData.logical_device.createPipelineLayout(
			vk::PipelineLayoutCreateInfo()
				.setSetLayoutCount(m_DescriptorLayout ? 1 : 0)
				.setPSetLayouts(m_DescriptorLayout ? &m_DescriptorLayout : nullptr)

				.setPushConstantRangeCount((u32)push_constant_ranges.size())
				.setPPushConstantRanges(push_constant_ranges.ptr())
		);

		vk::GraphicsPipelineCreateInfo create_info;

		create_info.stageCount = (u32)shader_infos.size();
		create_info.pStages = shader_infos.ptr();

		create_info.renderPass = renderer->window_data().render_pass();
		create_info.layout = m_Layout;

		create_info.pDynamicState = &dynamic_state_info;
		create_info.pViewportState = &viewport_info;
		create_info.pInputAssemblyState = &input_assembly_info;
		create_info.pVertexInputState = &vertex_input_info.pipeline_info;
		create_info.pRasterizationState = &rasterization_info;
		create_info.pMultisampleState = &multisample_info;
		create_info.pColorBlendState = &color_blend_info;
		create_info.pDepthStencilState = &depth_stencil_info;

		m_Pipeline = Internal::g_DeviceData.logical_device.createGraphicsPipeline(nullptr, create_info).value;
	}

	void Pipeline::destroy(void)
	{
		vk::Device logical_device = Internal::g_DeviceData.logical_device;

		logical_device.destroyDescriptorPool(m_DescriptorPool);
		logical_device.destroyPipeline(m_Pipeline);
		logical_device.destroyDescriptorSetLayout(m_DescriptorLayout);
		logical_device.destroyPipelineLayout(m_Layout);

		m_DynamicOffsets.~ArrayList();
	}

	void Pipeline::_set_descriptor_set(const Shaders& shaders, u32 max_frames_in_flight)
	{
		if (!shaders.size())
			return;

		for (View<const Graphics::Shader> shader : shaders)
		{
			for (const auto& [binding, uniform] : shader->uniforms())
			{
				switch (uniform->type())
				{
					case Graphics::UniformType::UniformBuffer:
					case Graphics::UniformType::StorageBuffer:
						m_DynamicOffsetCount++;
						break;
					default: 
						break;
				}
			}
		}

		m_DynamicOffsets.reallocate((u64)m_DynamicOffsetCount * max_frames_in_flight);
		m_DynamicOffsets.resize(m_DynamicOffsets.capacity());

		for (const View<const Graphics::Shader>& shader : shaders)
		{
			for (const auto& [binding, uniform] : shader->uniforms())
			{
				Graphics::UniformType type = uniform->type();
				if (type == Graphics::UniformType::None)
					continue;

				switch (type)
				{
				case Graphics::UniformType::Texture:
				{
					auto texture = static_ref_cast<const VulkanImpl::Texture>(uniform);

					vk::DescriptorImageInfo image_info;
					image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
					image_info.imageView = texture->img_view();
					image_info.sampler = texture->sampler();

					Internal::WriteToDescriptorSet(
						m_DescriptorSet,
						binding,
						vk::DescriptorType::eCombinedImageSampler,
						1,
						nullptr, // buffer info
						&image_info,
						nullptr // texel buffer view
					);
					break;
				}
				case Graphics::UniformType::UniformBuffer:
				{
					auto ubo = static_ref_cast<const VulkanImpl::UniformBuffer>(uniform);

					vk::DescriptorBufferInfo buffer_info(ubo->buffer().buffer, 0, ubo->aligned_size());

					Internal::WriteToDescriptorSet(
						m_DescriptorSet,
						binding,
						vk::DescriptorType::eUniformBufferDynamic,
						1,
						&buffer_info,
						nullptr,
						nullptr
					);

					for (u64 i = m_DynamicOffsetIndex++; i < m_DynamicOffsets.size(); i += m_DynamicOffsetCount)
						m_DynamicOffsets[i] = (u32)(ubo->aligned_size() * i);

					break;
				}
				case Graphics::UniformType::StorageBuffer:
				{
					auto ssbo = static_ref_cast<const VulkanImpl::StorageBuffer>(uniform);

					vk::DescriptorBufferInfo buffer_info(ssbo->buffer().buffer, 0, ssbo->aligned_size());

					Internal::WriteToDescriptorSet(
						m_DescriptorSet,
						binding,
						vk::DescriptorType::eStorageBufferDynamic,
						1,
						&buffer_info,
						nullptr,
						nullptr
					);

					for (u64 i = m_DynamicOffsetIndex++; i < m_DynamicOffsets.size(); i += m_DynamicOffsetCount)
						m_DynamicOffsets[i] = (u32)(ssbo->aligned_size() * i);

					break;
				}
				default:
					throw std::runtime_error("Failed to bind uniform to pipeline: Uniform object of unknown descriptor type!");
				}
			}
		}
	}

	/*
	void Pipeline::bind_uniform(u32 binding, View<const Graphics::Uniform> uniform)
	{
		Graphics::UniformType type = uniform->type();

		switch (type)
		{
			case Graphics::UniformType::Texture:
			{
				auto texture = static_ref_cast<const VulkanImpl::Texture>(uniform);

				vk::DescriptorImageInfo image_info;
				image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
				image_info.imageView = texture->img_view();
				image_info.sampler = texture->sampler();

				Internal::WriteToDescriptorSet(
					m_DescriptorSet,
					binding,
					vk::DescriptorType::eCombinedImageSampler,
					1,
					nullptr, // buffer info
					&image_info,
					nullptr // texel buffer view
				);

				break;
			}
			case Graphics::UniformType::UniformBuffer:
			{
				auto ubo = static_ref_cast<const VulkanImpl::UniformBuffer>(uniform);

				vk::DescriptorBufferInfo buffer_info(ubo->buffer().buffer, 0, ubo->aligned_size());

				Internal::WriteToDescriptorSet(
					m_DescriptorSet,
					binding,
					vk::DescriptorType::eUniformBufferDynamic,
					1, // count
					&buffer_info,
					nullptr, // image info
					nullptr // texel buffer view
				);

				for (u64 i = m_DynamicOffsetIndex++; i < m_DynamicOffsets.size(); i += m_DynamicOffsetCount)
					m_DynamicOffsets[i] = (u32)(ubo->aligned_size() * i);

				break;
			}
			case Graphics::UniformType::StorageBuffer:
			{
				auto ssbo = static_ref_cast<const VulkanImpl::StorageBuffer>(uniform);

				vk::DescriptorBufferInfo buffer_info(ssbo->buffer().buffer, 0, ssbo->aligned_size());

				Internal::WriteToDescriptorSet(
					m_DescriptorSet,
					binding,
					vk::DescriptorType::eStorageBufferDynamic,
					1, // count
					&buffer_info,
					nullptr, // image info
					nullptr // texel buffer view
				);

				for (u64 i = m_DynamicOffsetIndex++; i < m_DynamicOffsets.size(); i += m_DynamicOffsetCount)
					m_DynamicOffsets[i] = (u32)(ssbo->aligned_size() * i);

				break;
			}
			default:
				throw std::runtime_error("Failed to bind uniform to pipeline: Uniform object of unknown descriptor type!");
		}
	}
	*/

	Pipeline::Pipeline(Pipeline&& other)
	: m_Pipeline(std::exchange(other.m_Pipeline, nullptr)),

	m_DescriptorLayout(std::exchange(other.m_DescriptorLayout, nullptr)),
	m_Layout(std::exchange(other.m_Layout, nullptr)),

	m_DescriptorPool(std::exchange(other.m_DescriptorPool, nullptr)),
	m_DescriptorSet(std::exchange(other.m_DescriptorSet, nullptr)),
	m_DynamicOffsets(std::move(other.m_DynamicOffsets)),
	m_DynamicOffsetCount(other.m_DynamicOffsetCount),
	m_DynamicOffsetIndex(other.m_DynamicOffsetIndex)
	{}

	Pipeline& Pipeline::operator=(Pipeline&& other)
	{
		this->destroy();

		m_Pipeline = std::exchange(other.m_Pipeline, nullptr);

		m_DescriptorLayout = std::exchange(other.m_DescriptorLayout, nullptr);
		m_Layout = std::exchange(other.m_Layout, nullptr);

		m_DescriptorPool = std::exchange(other.m_DescriptorPool, nullptr);
		m_DescriptorSet = std::exchange(other.m_DescriptorSet, nullptr);

		m_DynamicOffsets = std::move(other.m_DynamicOffsets);
		m_DynamicOffsetCount = other.m_DynamicOffsetCount;
		m_DynamicOffsetIndex = other.m_DynamicOffsetIndex;

		return *this;
	}
} // namespace Na
