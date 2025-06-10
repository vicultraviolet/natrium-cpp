#include "Pch.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

#include "Natrium/Graphics/VkContext.hpp"

#include "./PipelineStates.hpp"

#include "Internal.hpp"
#include "Natrium/Graphics/Buffers/UniformBuffer.hpp"
#include "Natrium/Graphics/Buffers/StorageBuffer.hpp"
#include "Natrium/Graphics/Texture.hpp"

namespace Na {
	static std::tuple<
		Na::ArrayList<vk::VertexInputBindingDescription>,
		Na::ArrayList<vk::VertexInputAttributeDescription>
	>
		GetVertexInputInfo(
			const ShaderAttributeLayout& vertex_buffer_layout
		)
	{
		if (!vertex_buffer_layout.size())
			return { {}, {} };

		Na::ArrayList<vk::VertexInputBindingDescription> binding_descriptions(vertex_buffer_layout.size());
		binding_descriptions.resize(binding_descriptions.capacity());

		u64 attribute_count = 0;
		for (const auto& binding : vertex_buffer_layout)
			attribute_count += binding.attributes.size();

		Na::ArrayList<vk::VertexInputAttributeDescription> attribute_descriptions(attribute_count);
		attribute_descriptions.resize(attribute_descriptions.capacity());

		for (u32 i = 0; const auto& binding : vertex_buffer_layout)
		{
			u32 offset = 0;
			for (u64 j = 0; const auto& attribute : binding.attributes)
			{
				attribute_descriptions[i + j * binding_descriptions.size()].binding = binding.binding;
				attribute_descriptions[i + j * binding_descriptions.size()].location = attribute.location;
				attribute_descriptions[i + j * binding_descriptions.size()].format = (vk::Format)attribute.type;
				attribute_descriptions[i + j * binding_descriptions.size()].offset = offset;

				offset += SizeOf(attribute.type);
				j++;
			}

			binding_descriptions[i].binding = i;
			binding_descriptions[i].stride = offset;
			binding_descriptions[i].inputRate = (vk::VertexInputRate)binding.input_rate;

			i++;
		}

		return { binding_descriptions, attribute_descriptions };
	}

	static vk::DescriptorSetLayout createDescriptorSetLayout(const ShaderUniformLayout& descriptor_layout)
	{
		Na::ArrayList<vk::DescriptorSetLayoutBinding> bindings(descriptor_layout.size());
		bindings.resize(bindings.capacity());

		for (size_t i = 0; const auto& binding : descriptor_layout)
		{
			bindings[i].binding            = binding.binding;
			bindings[i].descriptorType     = (vk::DescriptorType)binding.type;
			bindings[i].stageFlags         = (vk::ShaderStageFlagBits)binding.shader_stage;
			bindings[i].descriptorCount    = 1;
			bindings[i].pImmutableSamplers = nullptr;

			i++;
		}

		vk::DescriptorSetLayoutCreateInfo create_info;
		create_info.bindingCount = (u32)bindings.size();
		create_info.pBindings = bindings.ptr();

		return VkContext::Get().logical_device().createDescriptorSetLayout(create_info);
	}

	static vk::DescriptorPool createDescriptorPool(const ShaderUniformLayout& descriptor_layout)
	{
		Na::ArrayList<vk::DescriptorPoolSize> pool_sizes(descriptor_layout.size());
		pool_sizes.resize(pool_sizes.capacity());

		for (size_t i = 0; const ShaderUniform& uniform : descriptor_layout)
		{
			pool_sizes[i].descriptorCount = 1; // 1 * uniform.count
			pool_sizes[i].type = (vk::DescriptorType)uniform.type;
			i++;
		}

		vk::DescriptorPoolCreateInfo create_info;
		create_info.poolSizeCount = (u32)pool_sizes.size();
		create_info.pPoolSizes = pool_sizes.ptr();
		create_info.maxSets = 1; // 1 * uniform.count

		return VkContext::Get().logical_device().createDescriptorPool(create_info);
	}

	static vk::DescriptorSet createDescriptorSet(vk::DescriptorSetLayout& layout, vk::DescriptorPool pool)
	{
		vk::DescriptorSetAllocateInfo alloc_info;
		alloc_info.descriptorPool = pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &layout;

		vk::DescriptorSet descriptor_set;

		vk::Result result = VkContext::Get().logical_device().allocateDescriptorSets(&alloc_info, &descriptor_set);
		NA_VERIFY_VK(result, "Failed to allocate descriptor set!");

		return descriptor_set;
	}

	static Na::ArrayList<vk::DescriptorSet> createDescriptorSets(u32 count, vk::DescriptorSetLayout* layouts, vk::DescriptorPool pool)
	{
		vk::DescriptorSetAllocateInfo alloc_info;
		alloc_info.descriptorPool = pool;
		alloc_info.descriptorSetCount = count;
		alloc_info.pSetLayouts = layouts;

		Na::ArrayList<vk::DescriptorSet> descriptor_sets(count);
		descriptor_sets.resize(descriptor_sets.capacity());

		vk::Result result = VkContext::Get().logical_device().allocateDescriptorSets(&alloc_info, descriptor_sets.ptr());
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
			if (
				uniform.type == ShaderUniformType::StorageBuffer ||
				uniform.type == ShaderUniformType::UniformBuffer
			)
				m_DynamicOffsetCount++;
		}
		m_DynamicOffsets.reallocate(u64(m_DynamicOffsetCount * renderer_core.settings()->max_frames_in_flight()));
		m_DynamicOffsets.resize(m_DynamicOffsets.capacity());

		Na::ArrayList<vk::DynamicState> dynamic_states = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		auto [binding_descriptions, attribute_descriptions] = GetVertexInputInfo(vertex_buffer_layout);

		vk::PipelineVertexInputStateCreateInfo vertex_input_info;
		vertex_input_info.vertexAttributeDescriptionCount = (u32)attribute_descriptions.size();
		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.ptr();
		vertex_input_info.vertexBindingDescriptionCount = (u32)binding_descriptions.size();
		vertex_input_info.pVertexBindingDescriptions = binding_descriptions.ptr();

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
			push_constant_ranges[i].stageFlags = (vk::ShaderStageFlagBits)push_constant.shader_stage;
			push_constant_ranges[i].offset = push_constant.offset;
			push_constant_ranges[i].size = push_constant.size;
			i++;
		}

		m_Layout = VkContext::Get().logical_device().createPipelineLayout(
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
		create_info.pVertexInputState = &vertex_input_info;
		create_info.pRasterizationState = &rasterization_info;
		create_info.pMultisampleState = &multisample_info;
		create_info.pColorBlendState = &color_blend_info;
		create_info.pDepthStencilState = &depth_stencil_info;

		m_Pipeline = VkContext::Get().logical_device().createGraphicsPipeline(nullptr, create_info).value;

		if (uniform_data_layout.size())
		{
			m_DescriptorPool = createDescriptorPool(uniform_data_layout);
			m_DescriptorSet = createDescriptorSet(m_DescriptorLayout, m_DescriptorPool);
		}
	}

	void GraphicsPipeline::destroy(void)
	{
		vk::Device logical_device = VkContext::Get().logical_device();

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
