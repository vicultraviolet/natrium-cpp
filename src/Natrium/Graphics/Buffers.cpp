#include "Pch.hpp"
#include "Natrium/Graphics/Buffers.hpp"

#include "Natrium/Graphics/VulkanImpl/vVertexBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vIndexBuffer.hpp"

namespace Na::Graphics {
	UniqueRef<VertexBuffer> VertexBuffer::Make(u64 size, const void* data)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::VertexBuffer>::Make(size, data);
		}
		return nullptr;
	}

	UniqueRef<IndexBuffer> IndexBuffer::Make(u32 count, const u32* data)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::IndexBuffer>::Make(count, data);
		}
		return nullptr;
	}
} // namespace Na::Graphics