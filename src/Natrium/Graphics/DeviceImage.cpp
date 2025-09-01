#include "Pch.hpp"
#include "Natrium/Graphics/DeviceImage.hpp"

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceImage.hpp"

namespace Na::Graphics {
	u8 ImageFormat_GetChannelCount(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::Rgba8:  return 4;
		case ImageFormat::Rgba32: return 4;
		}
		return 0;
	}

	u8 ImageFormat_GetPixelSize(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::Rgba8:  return 1 * ImageFormat_GetChannelCount(format);
		case ImageFormat::Rgba32: return 4 * ImageFormat_GetChannelCount(format);
		}
		return 0;
	}

	u32 ImageFormat_GetImageSize(ImageFormat format, u32 width, u32 height)
	{
		return width * height * ImageFormat_GetPixelSize(format);
	}

	UniqueRef<DeviceImage> DeviceImage::Make(const DeviceImageCreateInfo& info)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return MakeUnique<VulkanImpl::DeviceImage>(info);
		}
		return nullptr;
	}

	DeviceImage::DeviceImage(const DeviceImageCreateInfo& info)
	: m_Width(info.width),
      m_Height(info.height),
	  m_LayerCount(info.layer_count),
	  m_Format(info.format),
	  m_Type(info.type)
	{

	}

	DeviceImage::DeviceImage(DeviceImage&& other) noexcept
	: m_Width(std::exchange(other.m_Width, 0)),
	  m_Height(std::exchange(other.m_Height, 0)),

	  m_LayerCount(std::exchange(other.m_LayerCount, 0)),

	  m_Format(other.m_Format),
	  m_Type(other.m_Type)
	{

	}

	DeviceImage& DeviceImage::operator=(DeviceImage&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_Width = std::exchange(other.m_Width, 0);
		m_Height = std::exchange(other.m_Height, 0);

		m_LayerCount = std::exchange(other.m_LayerCount, 0);

		m_Format = other.m_Format;
		m_Type = other.m_Type;

		return *this;
	}
} // namespace Na::Graphics
