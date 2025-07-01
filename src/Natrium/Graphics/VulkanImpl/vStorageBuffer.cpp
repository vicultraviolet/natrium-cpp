#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vStorageBuffer.hpp"

#include "Internal.hpp"

namespace Na::VulkanImpl {
	StorageBuffer::StorageBuffer(u64 size, Ref<const RendererSettingsAsset> renderer_settings)
	: m_PerFrameSize(size)
	{
		const vk::DeviceSize alignment = Internal::g_DeviceData.physical_device.getProperties().limits.minUniformBufferOffsetAlignment;

		m_AlignedSize = (size + alignment - 1) & ~(alignment - 1);

		m_Buffer = DeviceBuffer(
			m_AlignedSize * renderer_settings->max_frames_in_flight(),
			vk::BufferUsageFlagBits::eStorageBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		m_Mapped = Internal::g_DeviceData.logical_device.mapMemory(m_Buffer.memory, 0, size);
	}

	void StorageBuffer::destroy(void)
	{
		m_Buffer.destroy();
	}

	StorageBuffer::StorageBuffer(StorageBuffer&& other) noexcept
	: m_Buffer(std::move(other.m_Buffer)),
	m_Mapped(std::exchange(other.m_Mapped, nullptr)),
	m_PerFrameSize(other.m_PerFrameSize),
	m_AlignedSize(other.m_AlignedSize)
	{}

	StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other) noexcept
	{
		m_Buffer = std::move(other.m_Buffer);
		m_Mapped = std::exchange(other.m_Mapped, nullptr);
		m_PerFrameSize = other.m_PerFrameSize;
		m_AlignedSize = other.m_AlignedSize;

		return *this;
	}
} // namespace Na
