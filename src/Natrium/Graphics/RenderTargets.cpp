#include "Pch.hpp"
#include "Natrium/Graphics/RenderTargets.hpp"

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VulkanImpl/vSwapchainRenderTarget.hpp"

namespace Na::Graphics {
	Ref<SwapchainRenderTarget> SwapchainRenderTarget::Make(
		WeakRef<const Window> window,
		Ref<const RendererSettings> renderer_settings
	)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return MakeRef<VulkanImpl::SwapchainRenderTarget>(window, renderer_settings);
		}

		return nullptr;
	}
} // namespace Na::Graphics