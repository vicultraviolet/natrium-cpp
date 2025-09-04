#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformSetLayout.hpp"

#include "Natrium/Graphics/VulkanImpl/vShader.hpp"
#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

namespace Na::VulkanImpl {
	vk::DescriptorType UniformTypeToVk(UniformType type)
	{
		switch (type)
		{
		case UniformType::UniformBuffer:      return vk::DescriptorType::eUniformBuffer;
		case UniformType::StorageBuffer:      return vk::DescriptorType::eStorageBuffer;
		case UniformType::UniformMultibuffer: return vk::DescriptorType::eUniformBufferDynamic;
		case UniformType::StorageMultibuffer: return vk::DescriptorType::eStorageBufferDynamic;
		case UniformType::StorageImage:       return vk::DescriptorType::eStorageImage;
		case UniformType::Texture:            return vk::DescriptorType::eCombinedImageSampler;
		}
		return vk::DescriptorType(0);
	}

	UniformSetLayout::UniformSetLayout(const UniformBinding* pbindings, u64 binding_count)
	{
		Na::ArrayList<vk::DescriptorSetLayoutBinding> bindings(binding_count, binding_count);
		for (u64 i = 0; i < binding_count; i++)
		{
			const UniformBinding& binding = pbindings[i];

			if (binding.type == UniformType::UniformMultibuffer ||
				binding.type == UniformType::StorageMultibuffer)
			{
				m_DynamicCount += binding.count;
			}

			bindings[i].binding = binding.binding;
			bindings[i].descriptorType = UniformTypeToVk(binding.type);
			bindings[i].descriptorCount = binding.count;
			bindings[i].stageFlags = ShaderStageToVk(binding.shader_stage);
			bindings[i].pImmutableSamplers = nullptr;
		}

		Na::ArrayList<vk::DescriptorBindingFlagsEXT> binding_flags;

		bool uniform_indexing_enabled = Device::Get()->extensions().contains(DeviceExtension::UniformIndexing);
		if (uniform_indexing_enabled)
		{
			binding_flags.reallocate(binding_count, binding_count);
			memset(binding_flags.ptr(), 0, sizeof(vk::DescriptorBindingFlagsEXT) * binding_count);

			for (u64 i = 0; i < binding_count; i++)
			{
				const UniformBinding& binding = pbindings[i];

				if (binding.partially_bound)
					binding_flags[i] |= vk::DescriptorBindingFlagBitsEXT::ePartiallyBound;

				if (binding.dynamic_count)
					binding_flags[i] |= vk::DescriptorBindingFlagBitsEXT::eVariableDescriptorCount;

				if (binding.update_unused_while_in_use)
					binding_flags[i] |= vk::DescriptorBindingFlagBitsEXT::eUpdateUnusedWhilePending;
			}
		}

		vk::DescriptorSetLayoutCreateInfo create_info;

		create_info.bindingCount = (u32)bindings.size();
		create_info.pBindings = bindings.ptr();

		if (!Device::Get()->uniform_indexing_info().update_after_bind_types.empty())
		{
			create_info.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;
		}

		vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT create_info_ex;

		create_info_ex.bindingCount = (u32)binding_flags.size();
		create_info_ex.pBindingFlags = binding_flags.ptr();

		if (uniform_indexing_enabled)
			create_info.pNext = &create_info_ex;

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
