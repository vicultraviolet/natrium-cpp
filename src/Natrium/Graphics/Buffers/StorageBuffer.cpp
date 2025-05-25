#include "Pch.hpp"
#include "Natrium/Graphics/Buffers/StorageBuffer.hpp"

#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Pipeline.hpp"
#include "Internal.hpp"

namespace Na {
	StorageBuffer::StorageBuffer(u64 size, AssetHandle<RendererSettings> renderer_settings)
	: m_PerFrameSize(size)
	{
		static VkDeviceSize x_Alignment = VkContext::GetPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment;

		m_AlignedSize = (size + x_Alignment - 1) & ~(x_Alignment - 1);

		m_Buffer = DeviceBuffer(
			m_AlignedSize * renderer_settings->max_frames_in_flight(),
			vk::BufferUsageFlagBits::eStorageBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		m_Mapped = VkContext::GetLogicalDevice().mapMemory(m_Buffer.memory, 0, size);
	}

	void StorageBuffer::destroy(void)
	{
		m_Buffer.destroy();
	}

	StorageBuffer::StorageBuffer(StorageBuffer&& other)
	: m_Buffer(std::move(other.m_Buffer)),
	m_Mapped(std::exchange(other.m_Mapped, nullptr)),
	m_PerFrameSize(other.m_PerFrameSize),
	m_AlignedSize(other.m_AlignedSize)
	{}

	StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other)
	{
		m_Buffer = std::move(other.m_Buffer);
		m_Mapped = std::exchange(other.m_Mapped, nullptr);
		m_PerFrameSize = other.m_PerFrameSize;
		m_AlignedSize = other.m_AlignedSize;

		return *this;
	}
} // namespace Na
