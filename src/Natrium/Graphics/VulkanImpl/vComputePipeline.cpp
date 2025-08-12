#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vComputePipeline.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"
#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformSetLayout.hpp"

namespace Na::VulkanImpl {
	ComputePipeline::ComputePipeline(
		View<const Graphics::Shader> _shader,
		const View<const Graphics::UniformSetLayout>* uniform_set_layouts,
		u64 uniform_set_layout_count
	)
	{
		const auto& logical_device = Device::Get()->logical_device();
		auto shader = static_ref_cast<const Shader>(_shader);

		vk::PushConstantRange push_constant_range;

		push_constant_range.stageFlags = vk::ShaderStageFlagBits::eCompute;
		push_constant_range.offset = 0;
		push_constant_range.size = shader->push_constant_size();

		ArrayList<vk::DescriptorSetLayout> descriptor_set_layouts(uniform_set_layout_count, uniform_set_layout_count);
		for (u64 i = 0; i < uniform_set_layout_count; i++)
		{
			descriptor_set_layouts[i] = static_ref_cast<UniformSetLayout>(uniform_set_layouts[i])->layout();
		}

		vk::PipelineLayoutCreateInfo layout_info;

		layout_info.pushConstantRangeCount = shader->has_push_constant();
		layout_info.pPushConstantRanges = &push_constant_range;

		layout_info.setLayoutCount = (u32)uniform_set_layout_count;
		layout_info.pSetLayouts = descriptor_set_layouts.ptr();

		m_Pipeline.layout = logical_device.createPipelineLayout(layout_info);

		vk::ComputePipelineCreateInfo create_info;

		create_info.stage.stage  = vk::ShaderStageFlagBits::eCompute;
		create_info.stage.module = shader->module();
		create_info.stage.pName  = shader->entry_point().data();

		create_info.layout       = m_Pipeline.layout;

		m_Pipeline.pipeline = logical_device.createComputePipeline(nullptr, create_info).value;
	}
} // namespace Na::VulkanImpl
