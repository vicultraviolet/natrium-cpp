#if !defined(NA_GRAPHICS_DEVICE_IMAGE_HPP)
#define NA_GRAPHICS_DEVICE_IMAGE_HPP

#include "Natrium/Core.hpp"

namespace Na::Graphics {
	enum class ImageFormat : u8 {
		None = 0,
		Rgba8, Rgba32
	};

	[[nodiscard]] u8 ImageFormat_GetChannelCount(ImageFormat format);
	[[nodiscard]] u8 ImageFormat_GetPixelSize(ImageFormat format);
	[[nodiscard]] u32 ImageFormat_GetImageSize(ImageFormat format, u32 width, u32 height);

	enum class DeviceImageTypeFlags : u8 {
		None            = 0,
		Sampled         = NA_BIT(0),
		Storage         = NA_BIT(1),
		ColorAttachment = NA_BIT(2),
		DepthAttachment = NA_BIT(3),

		All = Sampled | Storage | ColorAttachment | DepthAttachment
	};

	enum class DeviceImageStage : u8 {
		None = 0,
		Mutable,
		Texture,
		StorageImage
	};

	struct DeviceImageCreateInfo {
		u32 width, height;
		u32 layer_count = 1;

		ImageFormat format = ImageFormat::Rgba8;

		DeviceImageTypeFlags type = DeviceImageTypeFlags::None;
	};

	class DeviceImage {
	public:
		[[nodiscard]] static UniqueRef<DeviceImage> Make(const DeviceImageCreateInfo& info);
		virtual ~DeviceImage(void) = default;

		virtual void set_stage(DeviceImageStage stage) = 0;

		// will treat data as a single image and copy it into all layers
		virtual void set_all_data(const void* data, u32 starting_layer = 0, u32 layer_count = 1) = 0;

		// will treat data as an array of images and copy each one into a separate layer
		virtual void set_each_data(const void* data) = 0;

		virtual void set_each_data_2(const void* datas[]) = 0;

		[[nodiscard]] inline u32 width(void) const { return m_Width; }
		[[nodiscard]] inline u32 height(void) const { return m_Height; }

		[[nodiscard]] inline u32 layer_size(void) const { return ImageFormat_GetImageSize(m_Format, m_Width, m_Height); }
		[[nodiscard]] inline u32 total_size(void) const { return this->layer_size() * m_LayerCount; }

		[[nodiscard]] inline u32 layer_count(void) const { return m_LayerCount; }

		[[nodiscard]] inline ImageFormat format(void) const { return m_Format; }
		[[nodiscard]] inline DeviceImageTypeFlags type(void) const { return m_Type; }
	protected:

		DeviceImage(void) = default;
		DeviceImage(const DeviceImageCreateInfo& info);

		DeviceImage(DeviceImage&& other) noexcept;
		DeviceImage& operator=(DeviceImage&& other) noexcept;

	protected:
		u32 m_Width = 0, m_Height = 0;
		u32 m_LayerCount = 0;

		ImageFormat m_Format = ImageFormat::None;
		DeviceImageTypeFlags m_Type = DeviceImageTypeFlags::None;

		DeviceImageStage m_CurrentStage = DeviceImageStage::None;
	};

	inline DeviceImageTypeFlags operator|(DeviceImageTypeFlags lhs, DeviceImageTypeFlags rhs) { return (DeviceImageTypeFlags)((u8)lhs | (u8)rhs); }
	inline DeviceImageTypeFlags operator&(DeviceImageTypeFlags lhs, DeviceImageTypeFlags rhs) { return (DeviceImageTypeFlags)((u8)lhs & (u8)rhs); }
	inline DeviceImageTypeFlags operator^(DeviceImageTypeFlags lhs, DeviceImageTypeFlags rhs) { return (DeviceImageTypeFlags)((u8)lhs ^ (u8)rhs); }
	inline DeviceImageTypeFlags operator~(DeviceImageTypeFlags state) { return (DeviceImageTypeFlags)(~(u8)state); }

	inline DeviceImageTypeFlags& operator|=(DeviceImageTypeFlags& lhs, DeviceImageTypeFlags rhs) { lhs = lhs | rhs; return lhs; }
	inline DeviceImageTypeFlags& operator&=(DeviceImageTypeFlags& lhs, DeviceImageTypeFlags rhs) { lhs = lhs & rhs; return lhs; }
	inline DeviceImageTypeFlags& operator^=(DeviceImageTypeFlags& lhs, DeviceImageTypeFlags rhs) { lhs = lhs ^ rhs; return lhs; }

	inline bool operator==(DeviceImageTypeFlags lhs, DeviceImageTypeFlags rhs) { return (u8)lhs == (u8)rhs; }
	inline bool operator!=(DeviceImageTypeFlags lhs, DeviceImageTypeFlags rhs) { return (u8)lhs != (u8)rhs; }

	inline bool operator!(DeviceImageTypeFlags state) { return (u8)state == 0; }
} // namespace Na::Graphics

#endif // NA_GRAPHICS_DEVICE_IMAGE_HPP