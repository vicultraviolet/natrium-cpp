#include "Pch.hpp"
#include "Natrium/Graphics/Sampler.hpp"

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VulkanImpl/vSampler.hpp"

namespace Na::Graphics {
	UniqueRef<Sampler> Sampler::Make(const SamplerCreateInfo& info)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::Sampler>::Make(info);
		}
		return nullptr;
	}
} // namespace Na::Graphics
