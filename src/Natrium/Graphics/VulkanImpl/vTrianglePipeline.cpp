#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vTrianglePipeline.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

#include "./vPipelineStates.hpp"

#include "Internal.hpp"
#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformSetLayout.hpp"

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

	TrianglePipeline::TrianglePipeline(
		WeakRef<const Graphics::RenderTarget> _render_target,
		const Graphics::VertexAttributes& vertex_layout,
		const View<const Graphics::UniformSetLayout>* uniform_set_layouts,
		u64 uniform_set_layout_count,
		const View<const Graphics::Shader>* shaders,
		u64 shader_count
	)
	{
		const auto& logical_device = Device::Get()->logical_device();

		auto render_target = _render_target.lock();
		
		const Na::ArrayList<vk::DynamicState> dynamic_states = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		VertexInputInfo vertex_input_info(vertex_layout);

		auto dynamic_state_info = dynamicStateInfo(dynamic_states);
		auto viewport_info = viewportInfo();
		auto input_assembly_info = inputAssemblyInfo();
		auto rasterization_info = rasterizationInfo();
		auto multisample_info = multisampleInfo(render_target->renderer_settings()->multisampling_enabled);
		auto color_blend_attachment = colorBlendAttachment(false);
		auto color_blend_info = colorBlendInfo(color_blend_attachment);
		auto depth_stencil_info = depthStencilInfo();


		ArrayList<vk::PipelineShaderStageCreateInfo> shader_infos(shader_count, shader_count);
		u64 push_constant_count = 0;

		for (u64 i = 0; i < shader_count; i++)
		{
			auto shader = static_ref_cast<const Shader>(shaders[i]);

			shader_infos[i].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
			shader_infos[i].stage = ShaderStageToVk(shader->stage());
			shader_infos[i].module = shader->module();
			shader_infos[i].pName = shader->entry_point().data();
			shader_infos[i].pSpecializationInfo = nullptr; 
			shader_infos[i].pNext = nullptr;
			shader_infos[i].flags = vk::PipelineShaderStageCreateFlags();

			if (shader->has_push_constant())
				push_constant_count++;
		}

		Na::ArrayList<vk::PushConstantRange> push_constant_ranges(push_constant_count, push_constant_count);
		for (u64 i = 0; i < shader_count; i++)
		{
			if (!shaders[i]->has_push_constant())
				continue;

			push_constant_ranges[i].stageFlags = ShaderStageToVk(shaders[i]->stage());
			push_constant_ranges[i].offset = 0; 
			push_constant_ranges[i].size = shaders[i]->push_constant_size();
		}

		ArrayList<vk::DescriptorSetLayout> descriptor_set_layouts(uniform_set_layout_count, uniform_set_layout_count);
		for (u64 i = 0; i < uniform_set_layout_count; i++)
		{
			descriptor_set_layouts[i] = static_ref_cast<UniformSetLayout>(uniform_set_layouts[i])->layout();
		}

		vk::PipelineLayoutCreateInfo layout_info;

		layout_info.pushConstantRangeCount = (u32)push_constant_count;
		layout_info.pPushConstantRanges = push_constant_ranges.ptr();

		layout_info.setLayoutCount = (u32)uniform_set_layout_count;
		layout_info.pSetLayouts = descriptor_set_layouts.ptr();

		m_Pipeline.layout = logical_device.createPipelineLayout(layout_info);

		vk::GraphicsPipelineCreateInfo create_info;

		create_info.stageCount = (u32)shader_count;
		create_info.pStages = shader_infos.ptr();

		switch (render_target->type())
		{
		case Graphics::RenderTargetType::Swapchain:
			create_info.renderPass = static_ref_cast<const SwapchainRenderTarget>(render_target)->render_pass();
			break;
		}

		create_info.layout = m_Pipeline.layout;

		create_info.pDynamicState = &dynamic_state_info;
		create_info.pViewportState = &viewport_info;
		create_info.pInputAssemblyState = &input_assembly_info;
		create_info.pVertexInputState = &vertex_input_info.pipeline_info;
		create_info.pRasterizationState = &rasterization_info;
		create_info.pMultisampleState = &multisample_info;
		create_info.pColorBlendState = &color_blend_info;
		create_info.pDepthStencilState = &depth_stencil_info;

		m_Pipeline.pipeline = logical_device.createGraphicsPipeline(nullptr, create_info).value;
	}
} // namespace Na
