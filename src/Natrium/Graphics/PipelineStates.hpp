#include "Pch.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

namespace Na {
	static vk::PipelineDynamicStateCreateInfo dynamicStateInfo(const Na::ArrayVector<vk::DynamicState>& states)
	{
		vk::PipelineDynamicStateCreateInfo dynamic_state_info;
		dynamic_state_info.dynamicStateCount = (u32)states.size();
		dynamic_state_info.pDynamicStates = states.ptr();
		return dynamic_state_info;
	}

	static vk::PipelineViewportStateCreateInfo viewportInfo(void)
	{
		vk::PipelineViewportStateCreateInfo viewport_state_info;
		viewport_state_info.viewportCount = 1;
		viewport_state_info.scissorCount = 1;
		return viewport_state_info;
	}

	static vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo(void)
	{
		vk::PipelineInputAssemblyStateCreateInfo input_assembly_info;
		input_assembly_info.topology = vk::PrimitiveTopology::eTriangleList;
		input_assembly_info.primitiveRestartEnable = VK_FALSE;
		return input_assembly_info;
	}

	static vk::PipelineRasterizationStateCreateInfo rasterizationInfo(void)
	{
		vk::PipelineRasterizationStateCreateInfo rasterization_info;

		rasterization_info.depthClampEnable = VK_FALSE;
		rasterization_info.rasterizerDiscardEnable = VK_FALSE;

		rasterization_info.polygonMode = vk::PolygonMode::eFill;
		rasterization_info.lineWidth = 1.0f;

		rasterization_info.cullMode = vk::CullModeFlagBits::eBack;
		rasterization_info.frontFace = vk::FrontFace::eCounterClockwise;

		rasterization_info.depthBiasEnable = VK_FALSE;
		rasterization_info.depthBiasConstantFactor = 0.0f;
		rasterization_info.depthBiasClamp = 0.0f;
		rasterization_info.depthBiasSlopeFactor = 0.0f;

		return rasterization_info;
	}

	static vk::PipelineMultisampleStateCreateInfo multisampleInfo(bool enabled)
	{
		vk::PipelineMultisampleStateCreateInfo multisample_info;

		multisample_info.rasterizationSamples = VkContext::GetMSAASamples(enabled);

		multisample_info.sampleShadingEnable = enabled;
		multisample_info.minSampleShading = enabled ? 0.2f : 0.0f;

		multisample_info.pSampleMask = nullptr;
		multisample_info.alphaToCoverageEnable = VK_FALSE;
		multisample_info.alphaToOneEnable = VK_FALSE;
		return multisample_info;
	}

	static vk::PipelineColorBlendAttachmentState colorBlendAttachment(bool enabled)
	{
		vk::PipelineColorBlendAttachmentState color_blend_attachment;
		color_blend_attachment.colorWriteMask =
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA;

		color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;

		if ((color_blend_attachment.blendEnable = enabled))
		{
			color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
			color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
			color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
		} else
		{
			color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eOne;
			color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eZero;
			color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
		}

		return color_blend_attachment;
	}

	static vk::PipelineColorBlendStateCreateInfo colorBlendInfo(vk::PipelineColorBlendAttachmentState& color_blend_attachment)
	{
		vk::PipelineColorBlendStateCreateInfo color_blend_info;
		color_blend_info.logicOpEnable = VK_FALSE;
		color_blend_info.logicOp = vk::LogicOp::eCopy;
		color_blend_info.attachmentCount = 1;
		color_blend_info.pAttachments = &color_blend_attachment;
		color_blend_info.blendConstants[0] = 0.0f;
		color_blend_info.blendConstants[1] = 0.0f;
		color_blend_info.blendConstants[2] = 0.0f;
		color_blend_info.blendConstants[3] = 0.0f;
		return color_blend_info;
	}

	static vk::PipelineDepthStencilStateCreateInfo depthStencilInfo(void)
	{
		vk::PipelineDepthStencilStateCreateInfo depth_stencil_info;
		depth_stencil_info.depthTestEnable = VK_TRUE;
		depth_stencil_info.depthWriteEnable = VK_TRUE;
		depth_stencil_info.depthCompareOp = vk::CompareOp::eLess;
		depth_stencil_info.minDepthBounds = 0.0f;
		depth_stencil_info.maxDepthBounds = 1.0f;
		depth_stencil_info.stencilTestEnable = VK_FALSE;
		return depth_stencil_info;
	}
} // namespace Na
