#include "Pch.hpp"
#include "Natrium/Graphics/Texture.hpp"

#include "Natrium/Graphics/VulkanImpl/vTexture.hpp"

namespace Na::Graphics {
	UniqueRef<Texture> Texture::Make(
		const Ref<const ImageAsset>* imgs,
		u32 count,
		Ref<const RendererSettingsAsset> renderer_settings
	)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan:
			return UniqueRef<VulkanImpl::Texture>::Make(imgs, count, renderer_settings);
		}
		return nullptr;
	}

	UniqueRef<Texture> Texture::Make(
		Ref<const ImageAsset> img,
		Ref<const RendererSettingsAsset> renderer_settings
	)
	{
		return Texture::Make(&img, 1, renderer_settings);
	}

	UniqueRef<Texture> Texture::Make(
		const std::initializer_list<Ref<const ImageAsset>>& imgs,
		Ref<const RendererSettingsAsset> renderer_settings
	)
	{
		return Texture::Make(imgs.begin(), (u32)imgs.size(), renderer_settings);
	}
} // namespace Na::Graphics
