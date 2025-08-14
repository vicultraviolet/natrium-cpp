#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"
#include "Internal.hpp"

namespace Na::VulkanImpl {
	vk::BufferUsageFlags BufferTypeToVk(BufferTypeFlags type)
	{
		vk::BufferUsageFlags usage;

		if ((type & BufferTypeFlags::StorageBuffer) != BufferTypeFlags::None)
			usage |= vk::BufferUsageFlagBits::eStorageBuffer;

		if ((type & BufferTypeFlags::UniformBuffer) != BufferTypeFlags::None)
			usage |= vk::BufferUsageFlagBits::eUniformBuffer;

		if ((type & BufferTypeFlags::IndexBuffer) != BufferTypeFlags::None)
			usage |= vk::BufferUsageFlagBits::eIndexBuffer;

		if ((type & BufferTypeFlags::VertexBuffer) != BufferTypeFlags::None)
			usage |= vk::BufferUsageFlagBits::eVertexBuffer;

		return usage;
	}

	u32 FindMemoryType(u32 type_filter, vk::MemoryPropertyFlags user_props)
	{
		vk::PhysicalDeviceMemoryProperties memory_props = Device::Get()->physical_device().getMemoryProperties();

		for (u32 i = 0; i < memory_props.memoryTypeCount; i++)
			if ((type_filter & (1 << i)) && (memory_props.memoryTypes[i].propertyFlags & user_props) == user_props)
				return i;

		return 0;
	}

	Buffer::Buffer(const BufferCreateInfo& info)
	: Graphics::Buffer(info),
      m_ElementSize(info.size),
	  m_Count(info.count)
	{
		const auto& logical_device = Device::Get()->logical_device();

		if ((info.type & BufferTypeFlags::UniformBuffer | BufferTypeFlags::StorageBuffer) != BufferTypeFlags::None)
		{
			auto physical_device_props = Device::Get()->physical_device().getProperties();

			vk::DeviceSize alignment = std::max(
				physical_device_props.limits.minStorageBufferOffsetAlignment,
				physical_device_props.limits.minUniformBufferOffsetAlignment
			);

			m_AlignedSize = (info.size + alignment - 1) & ~(alignment - 1);
		} else
		{
			m_AlignedSize = info.size;
		}

		m_TotalSize = m_AlignedSize * info.count;

		vk::BufferUsageFlags usage = BufferTypeToVk(info.type);
		vk::MemoryPropertyFlags memory_props;

		if (info.cpu_accessible)
		{
			memory_props |= vk::MemoryPropertyFlagBits::eHostVisible;
			memory_props |= vk::MemoryPropertyFlagBits::eHostCoherent;
		} else
		{
			memory_props |= vk::MemoryPropertyFlagBits::eDeviceLocal;

			if (info.type != BufferTypeFlags::None)
				usage |= vk::BufferUsageFlagBits::eTransferDst;
		}

		vk::BufferCreateInfo buffer_info;
		buffer_info.size = m_TotalSize;
		buffer_info.usage = usage;
		buffer_info.sharingMode = vk::SharingMode::eExclusive;

		m_Buffer = logical_device.createBuffer(buffer_info);

		vk::MemoryRequirements memory_requirements = logical_device.getBufferMemoryRequirements(m_Buffer);

		vk::MemoryAllocateInfo alloc_info;
		alloc_info.allocationSize = memory_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, memory_props);

		m_Memory = logical_device.allocateMemory(alloc_info);
		logical_device.bindBufferMemory(m_Buffer, m_Memory, 0);
	}

	Buffer::Buffer(const BufferCreateInfo2& info)
	: m_TotalSize(info.size * info.count),
	  m_Count(info.count),
	  m_ElementSize(info.size),
	  m_AlignedSize(info.size)
	{
		const auto& logical_device = Device::Get()->logical_device();

		vk::BufferCreateInfo buffer_info;
		buffer_info.size = m_TotalSize;
		buffer_info.usage = info.usage;
		buffer_info.sharingMode = info.sharing_mode;

		m_Buffer = logical_device.createBuffer(buffer_info);

		vk::MemoryRequirements memory_requirements = logical_device.getBufferMemoryRequirements(m_Buffer);

		vk::MemoryAllocateInfo alloc_info;
		alloc_info.allocationSize = memory_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, info.memory_props);

		m_Memory = logical_device.allocateMemory(alloc_info);
		logical_device.bindBufferMemory(m_Buffer, m_Memory, 0);
	}

	void Buffer::destroy(void)
	{
		const auto& logical_device = Device::Get()->logical_device();

		m_TotalSize = 0;
		m_ElementSize = 0;
		m_AlignedSize = 0;
		m_Count = 0;

		if (m_Buffer)
		{
			logical_device.destroyBuffer(m_Buffer);
			m_Buffer = nullptr;
		}

		if (m_Memory)
		{
			if (m_Mapped)
			{

				logical_device.unmapMemory(m_Memory);
				m_Mapped = nullptr;
			}

			logical_device.freeMemory(m_Memory);
			m_Memory = nullptr;
		}
	}

	void Buffer::set_data(const void* data)
	{
		this->set_data_x(data, 0, m_TotalSize);
	}

	void Buffer::set_data_x(const void* data, u64 offset, u64 size)
	{
		if (!data)
			return;

		const auto& logical_device = Device::Get()->logical_device();

		if (this->cpu_accessible())
		{
			if (m_Mapped)
			{
				memcpy(m_Mapped + offset, data, size);
			} else
			{
				void* memory = logical_device.mapMemory(m_Memory, offset, size);
				memcpy(memory, data, size);
				logical_device.unmapMemory(m_Memory);
			}
		} else
		{
			BufferCreateInfo2 info{
				.size = size,
				.count = 1,
				.usage = vk::BufferUsageFlagBits::eTransferSrc,
				.memory_props = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			};
			Buffer staging_buffer(info);

			staging_buffer.set_data(data);

			this->copy_x(staging_buffer, 0, offset, size);
		}
	}


	Byte* Buffer::map(void)
	{
		return this->map_x(0, m_TotalSize);
	}

	Byte* Buffer::map_x(u64 offset, u64 size)
	{
		return m_Mapped = (Byte*)Device::Get()->logical_device().mapMemory(m_Memory, offset, size);
	}

	void Buffer::unmap(void)
	{
		Device::Get()->logical_device().unmapMemory(m_Memory);
		m_Mapped = nullptr;
	}

	void Buffer::copy(const Buffer& other)
	{
		this->copy_x(other, 0, 0, other.m_TotalSize);
	}

	void Buffer::copy_x(const Buffer& other, u64 src_offset, u64 dst_offset, u64 size)
	{
		vk::CommandBuffer cmd_buffer = Internal::BeginSingleTimeCommands();

		vk::BufferCopy copy_region;
		copy_region.srcOffset = src_offset;
		copy_region.dstOffset = dst_offset;
		copy_region.size = size;

		cmd_buffer.copyBuffer(other.m_Buffer, m_Buffer, { copy_region });

		Internal::EndSingleTimeCommands(cmd_buffer);
	}
} // namespace Na::VulkanImpl
