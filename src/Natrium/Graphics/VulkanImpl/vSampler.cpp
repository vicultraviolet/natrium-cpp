#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vSampler.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

namespace Na::VulkanImpl {
	vk::Filter SamplerFilterToVk(SamplerFilter filter)
	{
		switch (filter)
		{
		case SamplerFilter::Nearest: return vk::Filter::eNearest;
		case SamplerFilter::Linear:  return vk::Filter::eLinear;
		}
		return vk::Filter::eNearest;
	}

	vk::SamplerAddressMode SamplerAddressModeToVk(SamplerAddressMode mode)
	{
		switch (mode)
		{
		case SamplerAddressMode::Repeat:          return vk::SamplerAddressMode::eRepeat;
		case SamplerAddressMode::MirroredRepeat:  return vk::SamplerAddressMode::eMirroredRepeat;
		case SamplerAddressMode::ClampToEdge:     return vk::SamplerAddressMode::eClampToEdge;
		case SamplerAddressMode::ClampToBorder:   return vk::SamplerAddressMode::eClampToBorder;
		case SamplerAddressMode::MirrorClampToEdge:return vk::SamplerAddressMode::eMirrorClampToEdge;
		}
		return vk::SamplerAddressMode::eRepeat;
	}

	Sampler::Sampler(const SamplerCreateInfo& info)
	{
		auto renderer_settings = info.renderer_settings.lock();

		vk::SamplerCreateInfo create_info;

		create_info.magFilter = SamplerFilterToVk(info.oversampling_filter);
		create_info.minFilter = SamplerFilterToVk(info.undersampling_filter);

		create_info.addressModeU = SamplerAddressModeToVk(info.adress_mode_u);
		create_info.addressModeV = SamplerAddressModeToVk(info.adress_mode_v);
		create_info.addressModeW = vk::SamplerAddressMode::eRepeat;

		create_info.anisotropyEnable = renderer_settings->anisotropy_enabled;
		create_info.maxAnisotropy = renderer_settings->max_anisotropy;

		create_info.borderColor = vk::BorderColor::eFloatOpaqueBlack;

		create_info.unnormalizedCoordinates = VK_FALSE;

		create_info.compareEnable = VK_FALSE;
		create_info.compareOp = vk::CompareOp::eAlways;

		create_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		create_info.mipLodBias = 0.0f;
		create_info.minLod = 0.0f;
		create_info.maxLod = 0.0f;

		m_Sampler = Device::Get()->logical_device().createSampler(create_info);
	}

	Sampler::~Sampler(void)
	{
		if (m_Sampler)
		{
			Device::Get()->logical_device().destroySampler(m_Sampler);
			m_Sampler = nullptr;
		}
	}
} // namespace Na::VulkanImpl
