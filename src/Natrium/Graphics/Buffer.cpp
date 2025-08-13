#include "Pch.hpp"
#include "Natrium/Graphics/Buffer.hpp"

#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"

namespace Na::Graphics {
	UniqueRef<Buffer> Buffer::Make(const BufferCreateInfo& info)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::Buffer>::Make(info);
		}
		return nullptr;
	}

	Buffer::Buffer(const BufferCreateInfo& info)
	: m_Type(info.type),
	  m_CpuAccessible(info.cpu_accessible)
	{

	}

	UniqueRef<Buffer> MakeVertexBuffer(u64 size)
	{
		BufferCreateInfo info{
			.size = size,
			.count = 1,
			.cpu_accessible = false,
			.type = BufferTypeFlags::VertexBuffer
		};
		return Buffer::Make(info);
	}

	UniqueRef<Buffer> MakeIndexBuffer(u32 count)
	{
		BufferCreateInfo info{
			.size = count * sizeof(u32),
			.count = 1,
			.cpu_accessible = false,
			.type = BufferTypeFlags::IndexBuffer
		};
		return Buffer::Make(info);
	}

	UniqueRef<Buffer> MakeUniformBuffer(u64 size, Ref<const RendererSettingsAsset> renderer_settings)
	{
		BufferCreateInfo info{
			.size = size,
			.count = renderer_settings->max_frames_in_flight(),
			.cpu_accessible = true,
			.type = BufferTypeFlags::UniformBuffer
		};
		return Buffer::Make(info);
	}

	UniqueRef<Buffer> MakeStorageBuffer(u64 size, Ref<const RendererSettingsAsset> renderer_settings)
	{
		BufferCreateInfo info{
			.size = size,
			.count = renderer_settings->max_frames_in_flight(),
			.cpu_accessible = true,
			.type = BufferTypeFlags::StorageBuffer
		};
		return Buffer::Make(info);
	}
} // namespace Na::Graphics
