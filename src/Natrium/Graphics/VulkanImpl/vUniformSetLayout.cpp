#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformSetLayout.hpp"

#include "Natrium/Graphics/VulkanImpl/vShader.hpp"
#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

namespace Na::VulkanImpl {
	UniformSetLayout::UniformSetLayout(const UniformBinding* pbindings, u64 binding_count)
	{
		Na::ArrayList<vk::DescriptorSetLayoutBinding> bindings(binding_count, binding_count);
		for (u64 i = 0; i < binding_count; i++)
		{
			const UniformBinding& binding = pbindings[i];

			if (binding.type == UniformType::UniformBuffer ||
				binding.type == UniformType::StorageBuffer)
			{
				m_DynamicCount++;
			}

			bindings[i].binding = binding.binding;
			bindings[i].descriptorType = UniformTypeToVk(binding.type);
			bindings[i].descriptorCount = 1;
			bindings[i].stageFlags = ShaderStageToVk(binding.shader_stage);
			bindings[i].pImmutableSamplers = nullptr;

			i++;
		}

		vk::DescriptorSetLayoutCreateInfo create_info;
		create_info.bindingCount = (u32)bindings.size();
		create_info.pBindings = bindings.ptr();

		m_Layout = Device::Get()->logical_device().createDescriptorSetLayout(create_info);
	}

	void UniformSetLayout::destroy(void)
	{
		if (!m_Layout)
			return;

		Device::Get()->logical_device().destroyDescriptorSetLayout(m_Layout);
		m_Layout = nullptr;
	}
} // namespace Na::VulkanImpl
