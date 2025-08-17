#include "Pch.hpp"
#include "Natrium/Graphics/UniformSet.hpp"

#include "Natrium/Graphics/VulkanImpl/vUniformSet.hpp"

namespace Na::Graphics {
	UniqueRef<UniformSet> UniformSet::Make(
		View<const UniformSetLayout> layout,
		View<const Renderer> renderer
	)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return MakeUnique<VulkanImpl::UniformSet>(layout, renderer);
		}
		return nullptr;
	}
} // namespace Na::Graphics
