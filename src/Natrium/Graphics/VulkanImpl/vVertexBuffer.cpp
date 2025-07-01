#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vVertexBuffer.hpp"

#include "Internal.hpp"

namespace Na::VulkanImpl {
	VertexBuffer::VertexBuffer(u64 size, const void* data)
	: m_Buffer(
		size,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	)
	{
		this->set_data(data);
	}

	void VertexBuffer::destroy(void)
	{
		m_Buffer.destroy();
	}

	void VertexBuffer::set_data(const void* data)
	{
		if (!data)
			return;

		NA_ASSERT(m_Buffer.size, "Failed to set VertexBuffer data: Buffer size must be greater than 0");

		vk::Device logical_device = Internal::g_DeviceData.logical_device;

		DeviceBuffer stage_buffer(
			m_Buffer.size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		{
			void* memory = logical_device.mapMemory(stage_buffer.memory, 0, m_Buffer.size);
			memcpy(memory, data, m_Buffer.size);
			logical_device.unmapMemory(stage_buffer.memory);
		}

		m_Buffer.copy(stage_buffer);
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
	: m_Buffer(std::move(other.m_Buffer))
	{}

	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
	{
		m_Buffer = std::move(other.m_Buffer);
		return *this;
	}
} // namespace Na
