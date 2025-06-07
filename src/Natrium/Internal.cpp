#include "Pch.hpp"
#include "Internal.hpp"

#include "Natrium/Graphics/VkContext.hpp"

namespace Na {
	void Internal::WriteToDescriptorSet(
		vk::DescriptorSet set,
		u32 binding,
		vk::DescriptorType type,
		u32 count,
		vk::DescriptorBufferInfo* buffer_info, 
		vk::DescriptorImageInfo* image_info,
		vk::BufferView* texel_buffer_view
	)
	{
		vk::WriteDescriptorSet descriptor_write;
		descriptor_write.dstSet = set;
		descriptor_write.dstBinding = binding;
		descriptor_write.dstArrayElement = 0;

		descriptor_write.descriptorType = type;
		descriptor_write.descriptorCount = 1;

		descriptor_write.pBufferInfo = buffer_info;
		descriptor_write.pImageInfo = image_info;
		descriptor_write.pTexelBufferView = texel_buffer_view;

		VkContext::Get().logical_device().updateDescriptorSets(
			1, &descriptor_write,
			0, nullptr // descriptor copy
		);
	}

	vk::Sampler Internal::CreateSampler(
		vk::Filter oversampling_filter,
		vk::Filter undersampling_filter,
		bool anisotropy_enabled,
		float max_anisotropy
	)
	{
		vk::SamplerCreateInfo create_info;

		create_info.magFilter = oversampling_filter;
		create_info.minFilter = undersampling_filter;

		create_info.addressModeU = vk::SamplerAddressMode::eRepeat;
		create_info.addressModeV = vk::SamplerAddressMode::eRepeat;
		create_info.addressModeW = vk::SamplerAddressMode::eRepeat;

		create_info.anisotropyEnable = anisotropy_enabled;
		create_info.maxAnisotropy = max_anisotropy;

		create_info.borderColor = vk::BorderColor::eIntOpaqueBlack;

		create_info.unnormalizedCoordinates = VK_FALSE;

		create_info.compareEnable = VK_FALSE;
		create_info.compareOp = vk::CompareOp::eAlways;

		create_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		create_info.mipLodBias = 0.0f;
		create_info.minLod = 0.0f;
		create_info.maxLod = 0.0f;

		return VkContext::Get().logical_device().createSampler(create_info);
	}
} // namespace Na
