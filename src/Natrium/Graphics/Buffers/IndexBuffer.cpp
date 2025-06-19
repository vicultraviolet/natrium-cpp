#include "Pch.hpp"
#include "Natrium/Graphics/Buffers/IndexBuffer.hpp"

#include "Internal.hpp"

namespace Na {
	IndexBuffer::IndexBuffer(u32 count, const u32* data)
	: m_Count(count),
	m_Buffer(
		count * sizeof(u32),
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	)
	{
		this->set_data(data);
	}

	void IndexBuffer::destroy(void)
	{
		m_Buffer.destroy();
	}

	void IndexBuffer::set_data(const u32* data)
	{
		vk::Device logical_device = Internal::g_DeviceData.logical_device;

		DeviceBuffer stage_buffer(
			m_Buffer.size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		void* memory = logical_device.mapMemory(stage_buffer.memory, 0, m_Buffer.size);
		memcpy(memory, data, m_Buffer.size);
		logical_device.unmapMemory(stage_buffer.memory);

		m_Buffer.copy(stage_buffer);

		stage_buffer.destroy();
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& other)
	: m_Buffer(std::move(other.m_Buffer)),
	m_Count(std::exchange(other.m_Count, 0))
	{}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other)
	{
		m_Buffer = std::move(other.m_Buffer);
		m_Count = std::exchange(other.m_Count, 0);
		return *this;
	}
} // namespace Na
