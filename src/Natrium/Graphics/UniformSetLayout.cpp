#include "Pch.hpp"
#include "Natrium/Graphics/UniformSetLayout.hpp"

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformSetLayout.hpp"

namespace Na::Graphics {
	UniqueRef<UniformSetLayout> UniformSetLayout::Make(
		const UniformBinding* bindings,
		u64 binding_count
	)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return MakeUnique<VulkanImpl::UniformSetLayout>(bindings, binding_count);
		}
		return nullptr;
	}

	UniqueRef<UniformSetLayout> UniformSetLayout::Make(const std::initializer_list<UniformBinding>& bindings)
	{
		return UniformSetLayout::Make(bindings.begin(), bindings.size());
	}
} // namespace Na::Graphics
