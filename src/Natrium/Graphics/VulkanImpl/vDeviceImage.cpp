#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceImage.hpp"

#include "Internal.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"
#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"

namespace Na::VulkanImpl {
	vk::Format ImageFormatToVk(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::Rgba8: return vk::Format::eR8G8B8A8Unorm;
		case ImageFormat::Rgba32: return vk::Format::eR32G32B32A32Sfloat;
		}
		return vk::Format::eUndefined;
	}

	vk::ImageUsageFlags DeviceImageTypeToVk(DeviceImageTypeFlags type)
	{
		vk::ImageUsageFlags usage;

		if ((type & DeviceImageTypeFlags::Sampled) != DeviceImageTypeFlags::None)
			usage |= vk::ImageUsageFlagBits::eSampled;

		if ((type & DeviceImageTypeFlags::Storage) != DeviceImageTypeFlags::None)
			usage |= vk::ImageUsageFlagBits::eStorage;

		if ((type & DeviceImageTypeFlags::ColorAttachment) != DeviceImageTypeFlags::None)
			usage |= vk::ImageUsageFlagBits::eColorAttachment;

		if ((type & DeviceImageTypeFlags::DepthAttachment) != DeviceImageTypeFlags::None)
			usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;

		return usage;
	}

	vk::Format FindSupportedFormat(
		const std::initializer_list<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	)
	{
		for (vk::Format format : candidates)
		{
			vk::FormatProperties properties = Device::Get()->physical_device().getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
				return format;
			else
				if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
					return format;
		}

		return vk::Format::eUndefined;
	}

	DeviceImage::DeviceImage(const DeviceImageCreateInfo& info)
	: Graphics::DeviceImage(info),
	  m_AspectMask(vk::ImageAspectFlagBits::eColor)
	{
		const auto& logical_device = Device::Get()->logical_device();

		vk::ImageCreateInfo create_info;

		create_info.imageType = vk::ImageType::e2D;

		create_info.extent = vk::Extent3D(info.width, info.height, 1);
		create_info.mipLevels = 1;
		create_info.arrayLayers = info.layer_count;

		create_info.format = ImageFormatToVk(info.format);

		create_info.tiling = vk::ImageTiling::eOptimal;
		create_info.initialLayout = vk::ImageLayout::eUndefined;

		create_info.usage = (
			vk::ImageUsageFlagBits::eTransferDst |
			DeviceImageTypeToVk(info.type)
		);

		create_info.sharingMode = vk::SharingMode::eExclusive;

		create_info.samples = vk::SampleCountFlagBits::e1;

		m_Image = logical_device.createImage(create_info);

		vk::MemoryRequirements memory_requirements = logical_device.getImageMemoryRequirements(m_Image);

		vk::MemoryAllocateInfo alloc_info;
		alloc_info.allocationSize = memory_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(
			memory_requirements.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		m_Memory = logical_device.allocateMemory(alloc_info);
		logical_device.bindImageMemory(m_Image, m_Memory, 0);

		m_ImageView = CreateImageView(
			m_Image,
			m_AspectMask,
			create_info.format,
			info.layer_count
		);
	}

	vk::ImageView CreateImageView(
		vk::Image img,
		vk::ImageAspectFlags aspect_mask,
		vk::Format format,
		u32 layer_count
	)
	{
		vk::ImageViewCreateInfo create_info;

		create_info.image = img;

		if (layer_count == 1)
		{
			create_info.viewType = vk::ImageViewType::e2D;
		}
		else if (layer_count > 1)
		{
			create_info.viewType = vk::ImageViewType::e2DArray;
		}
		else
		{
			throw std::runtime_error("Failed to create Image View: Invalid layer count!");
		}

		create_info.format = format;

		create_info.subresourceRange.aspectMask = aspect_mask;

		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;

		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = layer_count;

		return Device::Get()->logical_device().createImageView(create_info);
	}

	void DeviceImage::destroy(void)
	{
		const auto& logical_device = Device::Get()->logical_device();

		if (m_ImageView)
		{
			logical_device.destroyImageView(m_ImageView);
			m_ImageView = nullptr;
		}

		if (m_Image)
		{
			logical_device.destroyImage(m_Image);
			m_Image = nullptr;
		}

		if (m_Memory)
		{
			logical_device.freeMemory(m_Memory);
			m_Memory = nullptr;
		}
	}

	void DeviceImage::set_stage(DeviceImageStage stage)
	{
		vk::ImageMemoryBarrier barrier;

		barrier.oldLayout = m_CurrentLayout;
		//barrier.newLayout = layout;

		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;

		barrier.image = m_Image;
		barrier.subresourceRange = vk::ImageSubresourceRange{
			m_AspectMask,
			0, // starting mip level
			1, // mip level count
			0, // starting array layer
			m_LayerCount
		};

		vk::PipelineStageFlags execute_stage;
		vk::PipelineStageFlags wait_stage;

		switch (stage)
		{
			case DeviceImageStage::Mutable:
			{
				barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;

				barrier.srcAccessMask = m_CurrentAccessMask;
				barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

				execute_stage = vk::PipelineStageFlagBits::eTopOfPipe;
				wait_stage = vk::PipelineStageFlagBits::eTransfer;

				break;
			}
			case DeviceImageStage::Texture:
			{
				barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

				barrier.srcAccessMask = m_CurrentAccessMask;
				barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

				execute_stage = vk::PipelineStageFlagBits::eTransfer;
				wait_stage = vk::PipelineStageFlagBits::eFragmentShader;

				break;
			}
			case DeviceImageStage::StorageImage:
			{
				barrier.newLayout = vk::ImageLayout::eGeneral;

				barrier.srcAccessMask = m_CurrentAccessMask;
				barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;

				execute_stage = vk::PipelineStageFlagBits::eTopOfPipe;
				wait_stage = vk::PipelineStageFlagBits::eComputeShader;

				break;
			}
		}

		m_CurrentLayout = barrier.newLayout;
		m_CurrentAccessMask = barrier.srcAccessMask;

		vk::CommandBuffer cmd_buffer = Internal::BeginSingleTimeCommands();

		cmd_buffer.pipelineBarrier(
			execute_stage,
			wait_stage,
			{}, // dependency flags
			0, nullptr, // memory barriers
			0, nullptr, // buffer memory barriers
			1, &barrier // image memory barriers
		);

		Internal::EndSingleTimeCommands(cmd_buffer);
	}

	void DeviceImage::set_all_data(const void* data, u32 starting_layer, u32 layer_count)
	{
		BufferCreateInfo2 buffer_info{
			.size = this->layer_size(),
			.count = 1,
			.usage = vk::BufferUsageFlagBits::eTransferSrc,
			.memory_props = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};
		Buffer staging_buffer(buffer_info);

		staging_buffer.set_data(data);

		this->copy_from_buffer(staging_buffer.native(), starting_layer, layer_count);
	}

	void DeviceImage::set_each_data(const void* data)
	{
		BufferCreateInfo2 buffer_info{
			.size = this->total_size(),
			.count = 1,
			.usage = vk::BufferUsageFlagBits::eTransferSrc,
			.memory_props = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};
		Buffer staging_buffer(buffer_info);

		staging_buffer.set_data(data);

		this->copy_each_from_buffer(staging_buffer.native());
	}

	void DeviceImage::set_each_data_2(const void* datas[])
	{
		BufferCreateInfo2 buffer_info{
			.size = this->total_size(),
			.count = 1,
			.usage = vk::BufferUsageFlagBits::eTransferSrc,
			.memory_props = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};
		Buffer staging_buffer(buffer_info);

		for (u32 i = 0; i < m_LayerCount; i++)
		{
			staging_buffer.set_data(datas[i]);
		}

		this->copy_each_from_buffer(staging_buffer.native());
	}

	DeviceImage::DeviceImage(const DeviceImageCreateInfo2& info)
	: m_AspectMask(info.aspect_mask)
	{
		const auto& logical_device = Device::Get()->logical_device();

		vk::ImageCreateInfo create_info;

		if (info.extent.depth > 1)
			create_info.imageType = vk::ImageType::e3D;
		else
			create_info.imageType = vk::ImageType::e2D;

		create_info.extent = info.extent;
		create_info.mipLevels = 1;
		create_info.arrayLayers = info.layer_count;

		create_info.format = info.format;

		create_info.tiling = info.tiling;
		create_info.initialLayout = vk::ImageLayout::eUndefined;

		create_info.usage = info.usage;

		create_info.sharingMode = info.sharing_mode;

		create_info.samples = info.sample_count;

		m_Image = logical_device.createImage(create_info);

		vk::MemoryRequirements memory_requirements = logical_device.getImageMemoryRequirements(m_Image);

		vk::MemoryAllocateInfo alloc_info;
		alloc_info.allocationSize = memory_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(
			memory_requirements.memoryTypeBits,
			info.memory_properties
		);

		m_Memory = logical_device.allocateMemory(alloc_info);
		logical_device.bindImageMemory(m_Image, m_Memory, 0);

		m_ImageView = CreateImageView(
			m_Image,
			info.aspect_mask,
			info.format,
			info.layer_count
		);
	}

	void DeviceImage::copy_from_buffer(vk::Buffer buffer, u32 starting_layer, u32 layer_count)
	{
		vk::BufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = m_AspectMask;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = starting_layer;
		region.imageSubresource.layerCount = layer_count;

		region.imageOffset = { { 0, 0, 0 } };
		region.imageExtent = vk::Extent3D(m_Width, m_Height, 1);

		vk::CommandBuffer cmd_buffer = Internal::BeginSingleTimeCommands();

		cmd_buffer.copyBufferToImage(
			buffer,
			m_Image,
			vk::ImageLayout::eTransferDstOptimal,
			1, &region
		);

		Internal::EndSingleTimeCommands(cmd_buffer);
	}

	void DeviceImage::copy_each_from_buffer(vk::Buffer buffer)
	{
		Na::ArrayList<vk::BufferImageCopy> regions(m_LayerCount, m_LayerCount);

		for (u32 i = 0; i < regions.size(); i++)
		{
			regions[i].bufferImageHeight = 0;
			regions[i].bufferRowLength = 0;
			regions[i].bufferOffset = (u64)this->layer_size() * (u64)i;
			regions[i].imageSubresource = vk::ImageSubresourceLayers(
				vk::ImageAspectFlagBits::eColor,
				0, // mip level
				i, // layer
				1 // layer count (1 at a time)
			);

			regions[i].imageOffset = vk::Offset3D();
			regions[i].imageExtent = vk::Extent3D(m_Width, m_Height, 1);
		}

		vk::CommandBuffer cmd_buffer = Internal::BeginSingleTimeCommands();

		cmd_buffer.copyBufferToImage(
			buffer, // src
			m_Image, // dest
			vk::ImageLayout::eTransferDstOptimal,
			(u32)regions.size(), regions.ptr()
		);

		Internal::EndSingleTimeCommands(cmd_buffer);
	}

	void DeviceImage::copy_from_buffers(
		const vk::Buffer* buffers,
		u32 buffer_count,
		u32 starting_layer
	)
	{
		vk::BufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = m_AspectMask;

		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { { 0, 0, 0 } };
		region.imageExtent = vk::Extent3D(m_Width, m_Height, 1);

		vk::CommandBuffer cmd_buffer = Internal::BeginSingleTimeCommands();

		for (u32 i = starting_layer; i < starting_layer + buffer_count; i++)
		{
			region.imageSubresource.baseArrayLayer = i;

			cmd_buffer.copyBufferToImage(
				buffers[i],
				m_Image,
				vk::ImageLayout::eTransferDstOptimal,
				1, &region
			);
		}

		Internal::EndSingleTimeCommands(cmd_buffer);
	}

	DeviceImage::DeviceImage(DeviceImage&& other)
	: Graphics::DeviceImage(std::forward<DeviceImage>(other)),

	  m_Image(std::exchange(other.m_Image, nullptr)),
	  m_Memory(std::exchange(other.m_Memory, nullptr)),
	  
	  m_ImageView(std::exchange(other.m_ImageView, nullptr)),
	  
	  m_AspectMask(other.m_AspectMask)
	{

	}

	DeviceImage& DeviceImage::operator=(DeviceImage&& other)
	{
		if (this == &other)
			return *this;

		this->destroy();

		Graphics::DeviceImage::operator=(std::forward<DeviceImage>(other));

		m_Image = std::exchange(other.m_Image, nullptr);
		m_Memory = std::exchange(other.m_Memory, nullptr);

		m_ImageView = std::exchange(other.m_ImageView, nullptr);

		m_AspectMask = other.m_AspectMask;

		return *this;
	}
} // namespace Na
