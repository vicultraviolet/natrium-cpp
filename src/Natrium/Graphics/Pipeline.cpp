#include "Pch.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

#include "./PipelineStates.hpp"

#include "Internal.hpp"
#include "Natrium/Graphics/Buffers/UniformBuffer.hpp"
#include "Natrium/Graphics/Buffers/StorageBuffer.hpp"
#include "Natrium/Graphics/Texture.hpp"

namespace Na {
	static vk::Format enumToVulkan(ShaderAttributeType type)
	{
		switch (type)
		{
		case ShaderAttributeType::Float: return vk::Format::eR32Sfloat;
		case ShaderAttributeType::Vec2:  return vk::Format::eR32G32Sfloat;
		case ShaderAttributeType::Vec3:  return vk::Format::eR32G32B32Sfloat;
		case ShaderAttributeType::Vec4:  return vk::Format::eR32G32B32A32Sfloat;
		}
		return vk::Format::eUndefined;
	}

	static u32 sizeOf(ShaderAttributeType type)
	{
		switch (type)
		{
		case ShaderAttributeType::Float:  return sizeof(float);
		case ShaderAttributeType::Vec2:   return sizeof(float) * 2;
		case ShaderAttributeType::Vec3:   return sizeof(float) * 3;
		case ShaderAttributeType::Vec4:   return sizeof(float) * 4;
		}
		return 0;
	}

	static vk::VertexInputRate enumToVulkan(ShaderAttributeInputRate input_rate)
	{
		switch (input_rate)
		{
		case ShaderAttributeInputRate::Vertex:   return vk::VertexInputRate::eVertex;
		case ShaderAttributeInputRate::Instance: return vk::VertexInputRate::eInstance;
		}
		return vk::VertexInputRate::eVertex;
	}

	static vk::DescriptorType enumToVulkan(ShaderUniformType type)
	{
		switch (type)
		{
		case ShaderUniformType::UniformBuffer: return vk::DescriptorType::eUniformBufferDynamic;
		case ShaderUniformType::StorageBuffer: return vk::DescriptorType::eStorageBufferDynamic;
		case ShaderUniformType::Texture:       return vk::DescriptorType::eCombinedImageSampler;
		}
		return (vk::DescriptorType)k_I32Max;
	}

	struct VertexInputInfo {
		Na::ArrayList<vk::VertexInputBindingDescription> bindings;
		Na::ArrayList<vk::VertexInputAttributeDescription> attributes;

		vk::PipelineVertexInputStateCreateInfo pipeline_info;

		VertexInputInfo(void) = default;

		VertexInputInfo(const ShaderAttributeLayout& vertex_attribute_bindings)
		{
			u32 binding_count = (u32)vertex_attribute_bindings.size();

			if (binding_count == 0)
				return;

			bindings.reallocate(binding_count, binding_count);

			u64 attribute_count = 0;
			for (const auto& binding : vertex_attribute_bindings)
				attribute_count += binding.attributes.size();

			attributes.reallocate(attribute_count, attribute_count);

			for (u32 i = 0; const auto& binding : vertex_attribute_bindings)
			{
				u32 offset = 0;
				for (u64 j = 0; const auto& attribute : binding.attributes)
				{
					u64 index = i + j * bindings.size();

					attributes[index].binding = binding.binding;
					attributes[index].location = attribute.location;
					attributes[index].format = enumToVulkan(attribute.type);
					attributes[index].offset = offset;

					offset += sizeOf(attribute.type);
					j++;
				}

				bindings[i].binding = i;
				bindings[i].stride = offset;
				bindings[i].inputRate = enumToVulkan(binding.input_rate);

				i++;
			}

			pipeline_info.vertexBindingDescriptionCount = (u32)bindings.size();
			pipeline_info.pVertexBindingDescriptions = bindings.ptr();
			pipeline_info.vertexAttributeDescriptionCount = (u32)attributes.size();
			pipeline_info.pVertexAttributeDescriptions = attributes.ptr();
		}
	};

	static vk::DescriptorSetLayout createDescriptorSetLayout(const ShaderUniformLayout& uniform_bindings)
	{
		u64 binding_count = uniform_bindings.size();
		Na::ArrayList<vk::DescriptorSetLayoutBinding> bindings(binding_count, binding_count);

		for (u64 i = 0; const auto& binding : uniform_bindings)
		{
			bindings[i].binding            = binding.binding;
			bindings[i].descriptorType     = enumToVulkan(binding.type);
			bindings[i].stageFlags         = Internal::EnumToVulkan(binding.shader_stage);
			bindings[i].descriptorCount    = 1;
			bindings[i].pImmutableSamplers = nullptr;

			i++;
		}

		vk::DescriptorSetLayoutCreateInfo create_info;
		create_info.bindingCount = (u32)binding_count;
		create_info.pBindings = bindings.ptr();

		return Internal::g_DeviceData.logical_device.createDescriptorSetLayout(create_info);
	}

	static vk::DescriptorPool createDescriptorPool(const ShaderUniformLayout& uniform_bindings)
	{
		u64 binding_count = uniform_bindings.size();
		Na::ArrayList<vk::DescriptorPoolSize> pool_sizes(binding_count, binding_count);

		for (u64 i = 0; const ShaderUniform& binding : uniform_bindings)
		{
			pool_sizes[i].descriptorCount = 1; // uniform.count
			pool_sizes[i].type = enumToVulkan(binding.type);
			i++;
		}

		vk::DescriptorPoolCreateInfo create_info;
		create_info.poolSizeCount = (u32)pool_sizes.size();
		create_info.pPoolSizes = pool_sizes.ptr();
		create_info.maxSets = 1; // uniform.count

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

	GraphicsPipeline::GraphicsPipeline(
		RendererCore& renderer_core,
		const PipelineShaderInfos& shader_infos,
		const ShaderAttributeLayout& vertex_buffer_layout,
		const ShaderUniformLayout& uniform_data_layout,
		const PushConstantLayout& push_constant_layout
	)
	{
		for (const ShaderUniform& uniform : uniform_data_layout)
		{
			if (uniform.type == ShaderUniformType::StorageBuffer ||
				uniform.type == ShaderUniformType::UniformBuffer)
				m_DynamicOffsetCount++;
		}
		m_DynamicOffsets.reallocate((u64)m_DynamicOffsetCount * renderer_core.settings()->max_frames_in_flight());
		m_DynamicOffsets.resize(m_DynamicOffsets.capacity());

		Na::ArrayList<vk::DynamicState> dynamic_states = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		VertexInputInfo vertex_input_info(vertex_buffer_layout);

		auto dynamic_state_info = dynamicStateInfo(dynamic_states);
		auto viewport_info = viewportInfo();
		auto input_assembly_info = inputAssemblyInfo();
		auto rasterization_info = rasterizationInfo();
		auto multisample_info = multisampleInfo(renderer_core.settings()->multisampling_enabled());
		auto color_blend_attachment = colorBlendAttachment(false);
		auto color_blend_info = colorBlendInfo(color_blend_attachment);
		auto depth_stencil_info = depthStencilInfo();

		if (uniform_data_layout.size())
			m_DescriptorLayout = createDescriptorSetLayout(uniform_data_layout);

		Na::ArrayList<vk::PushConstantRange> push_constant_ranges(push_constant_layout.size());
		push_constant_ranges.resize(push_constant_ranges.capacity());

		for (u64 i = 0; const auto& push_constant : push_constant_layout)
		{
			push_constant_ranges[i].stageFlags = Internal::EnumToVulkan(push_constant.shader_stage);
			push_constant_ranges[i].offset = push_constant.offset;
			push_constant_ranges[i].size = push_constant.size;
			i++;
		}

		m_Layout = Internal::g_DeviceData.logical_device.createPipelineLayout(
			vk::PipelineLayoutCreateInfo(
				{}, // flags
				(bool)m_DescriptorLayout, uniform_data_layout.size() ? &m_DescriptorLayout : nullptr,
				(u32)push_constant_ranges.size(), push_constant_ranges.ptr()
			)
		);

		vk::GraphicsPipelineCreateInfo create_info;

		create_info.stageCount = (u32)shader_infos.size();
		create_info.pStages = shader_infos.begin();

		create_info.renderPass = renderer_core.render_pass();
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

		if (uniform_data_layout.size())
		{
			m_DescriptorPool = createDescriptorPool(uniform_data_layout);
			m_DescriptorSet = createDescriptorSet(m_DescriptorLayout, m_DescriptorPool);
		}
	}

	void GraphicsPipeline::destroy(void)
	{
		vk::Device logical_device = Internal::g_DeviceData.logical_device;

		logical_device.destroyDescriptorPool(m_DescriptorPool);
		logical_device.destroyPipeline(m_Pipeline);
		logical_device.destroyDescriptorSetLayout(m_DescriptorLayout);
		logical_device.destroyPipelineLayout(m_Layout);

		m_DynamicOffsets.~ArrayList();
	}

	void GraphicsPipeline::_bind_uniform(u32 binding, const void* uniform)
	{
		ShaderUniformType uniform_type = *(const ShaderUniformType*)uniform;
		switch (uniform_type)
		{
			case ShaderUniformType::Texture:
			{
				const Texture& texture = *(const Texture*)uniform;
				vk::DescriptorImageInfo image_info;
				image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
				image_info.imageView = texture.img_view();
				image_info.sampler = texture.sampler();

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
			case ShaderUniformType::UniformBuffer:
			{
				const UniformBuffer& uniform_buffer = *(const UniformBuffer*)uniform;
				vk::DescriptorBufferInfo buffer_info(uniform_buffer.buffer().buffer, 0, uniform_buffer.aligned_size());

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
					m_DynamicOffsets[i] = u32(uniform_buffer.aligned_size() * i);

				break;
			}
			case ShaderUniformType::StorageBuffer:
			{
				const StorageBuffer& storage_buffer = *(const StorageBuffer*)uniform;
				vk::DescriptorBufferInfo buffer_info(storage_buffer.buffer().buffer, 0, storage_buffer.aligned_size());

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
					m_DynamicOffsets[i] = u32(storage_buffer.aligned_size() * i);

				break;
			}
			default:
				throw std::runtime_error("Failed to bind uniform to pipeline: Uniform object of unknown descriptor type!");
		}
	}

	GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other)
	: m_Pipeline(std::exchange(other.m_Pipeline, nullptr)),

	m_DescriptorLayout(std::exchange(other.m_DescriptorLayout, nullptr)),
	m_Layout(std::exchange(other.m_Layout, nullptr)),

	m_DescriptorPool(std::exchange(other.m_DescriptorPool, nullptr)),
	m_DescriptorSet(std::exchange(other.m_DescriptorSet, nullptr)),
	m_DynamicOffsets(std::move(other.m_DynamicOffsets)),
	m_DynamicOffsetCount(other.m_DynamicOffsetCount),
	m_DynamicOffsetIndex(other.m_DynamicOffsetIndex)
	{}

	GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other)
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
