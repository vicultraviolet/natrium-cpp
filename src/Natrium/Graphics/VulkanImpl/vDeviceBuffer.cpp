#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceBuffer.hpp"

#include "Internal.hpp"

namespace Na::VulkanImpl {
	u32 FindMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties memory_properties = Internal::g_DeviceData.physical_device.getMemoryProperties();

		for (u32 i = 0; i < memory_properties.memoryTypeCount; i++)
			if ((typeFilter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		return 0;
	}

	DeviceBuffer::DeviceBuffer(
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::SharingMode sharing_mode
	)
	: size(size)
	{
		vk::Device logical_device = Internal::g_DeviceData.logical_device;

		vk::BufferCreateInfo buffer_info;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = sharing_mode;

		this->buffer = logical_device.createBuffer(buffer_info);

		vk::MemoryRequirements memory_requirements = logical_device.getBufferMemoryRequirements(this->buffer);

		vk::MemoryAllocateInfo alloc_info;
		alloc_info.allocationSize = memory_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, properties);

		this->memory = logical_device.allocateMemory(alloc_info);
		logical_device.bindBufferMemory(this->buffer, this->memory, 0);
	}

	void DeviceBuffer::destroy(void)
	{
		vk::Device logical_device = Internal::g_DeviceData.logical_device;

		this->size = 0;

		logical_device.destroyBuffer(this->buffer);
		this->buffer = nullptr;

		logical_device.freeMemory(this->memory);
		this->memory = nullptr;
	}

	void DeviceBuffer::copy(const DeviceBuffer& other) 
	{
		vk::CommandBuffer cmd_buffer = Internal::BeginSingleTimeCommands();

		vk::BufferCopy copy_region;
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = other.size;

		cmd_buffer.copyBuffer(other.buffer, this->buffer, 1, &copy_region);

		Internal::EndSingleTimeCommands(cmd_buffer);
	}

	DeviceBuffer::DeviceBuffer(DeviceBuffer&& other)
	: buffer(std::exchange(other.buffer, nullptr)),
	size(std::exchange(other.size, 0)),
	memory(std::exchange(other.memory, nullptr))
	{}

	DeviceBuffer& DeviceBuffer::operator=(DeviceBuffer&& other)
	{
		vk::Device logical_device = Internal::g_DeviceData.logical_device;

		logical_device.destroyBuffer(this->buffer);
		logical_device.freeMemory(this->memory);

		this->buffer = std::exchange(other.buffer, nullptr);
		this->size = std::exchange(other.size, 0);
		this->memory = std::exchange(other.memory, nullptr);

		return *this;
	}

} // namespace Na
