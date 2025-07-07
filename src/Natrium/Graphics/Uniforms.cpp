#include "Pch.hpp"
#include "Natrium/Graphics/Uniforms.hpp"

#include "Natrium/Graphics/VulkanImpl/vUniformBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vStorageBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vTexture.hpp"

namespace Na::Graphics {
	UniqueRef<UniformBuffer> UniformBuffer::Make(u64 size, Ref<const RendererSettingsAsset> renderer_settings)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan:
			return UniqueRef<VulkanImpl::UniformBuffer>::Make(size, renderer_settings);
		}
		return nullptr;
	}

	UniqueRef<StorageBuffer> StorageBuffer::Make(u64 size, Ref<const RendererSettingsAsset> renderer_settings)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan:
			return UniqueRef<VulkanImpl::StorageBuffer>::Make(size, renderer_settings);
		}
		return nullptr;
	}

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
} // namespace Na::Graphics
