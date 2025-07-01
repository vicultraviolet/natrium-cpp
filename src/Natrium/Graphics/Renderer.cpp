#include "Pch.hpp"
#include "Natrium/Graphics/Renderer.hpp"

#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"

namespace Na::Graphics {
	UniqueRef<Renderer> Renderer::Make(
		const Window& window,
		Ref<const RendererSettingsAsset> settings
	)
	{
		switch (Device::Get().backend())
		{
			case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::Renderer>::Make(window, settings);
		}
		return nullptr;
	}
} // namespace Na::Graphics
