#if !defined(NA_VULKAN_IMPL_DEVICE_IMAGE_HPP)
#define NA_VULKAN_IMPL_DEVICE_IMAGE_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Graphics/DeviceImage.hpp"

namespace Na::VulkanImpl {
	using DeviceImageCreateInfo = Graphics::DeviceImageCreateInfo;
	struct DeviceImageCreateInfo2 {
		vk::Extent3D extent;
		u32 layer_count;
		vk::ImageAspectFlagBits aspect_mask;
		vk::Format format;
		vk::ImageTiling tiling;
		vk::ImageUsageFlags usage;
		vk::SharingMode sharing_mode;
		vk::SampleCountFlagBits sample_count;
		vk::MemoryPropertyFlags memory_properties;
	};

	using ImageFormat = Graphics::ImageFormat;
	using DeviceImageTypeFlags = Graphics::DeviceImageTypeFlags;
	using DeviceImageStage = Graphics::DeviceImageStage;

	[[nodiscard]] vk::Format ImageFormatToVk(ImageFormat format);
	[[nodiscard]] vk::ImageUsageFlags DeviceImageTypeToVk(DeviceImageTypeFlags type);
	[[nodiscard]] vk::ImageLayout DeviceImageStageToVk(DeviceImageStage stage);

	[[nodiscard]] vk::ImageView CreateImageView(vk::Image img, vk::ImageAspectFlags aspect_mask, vk::Format format, u32 layer_count = 1);

	[[nodiscard]] vk::Format FindSupportedFormat(
		const std::initializer_list<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	);

	class DeviceImage : public Graphics::DeviceImage {
	public:
		DeviceImage(void) = default;
		~DeviceImage(void) { this->destroy(); }

		void destroy(void);

		DeviceImage(const DeviceImageCreateInfo& info);
		DeviceImage(const DeviceImageCreateInfo2& info);

		DeviceImage(DeviceImage&& other);
		DeviceImage& operator=(DeviceImage&& other);

		void set_stage(DeviceImageStage stage) override;

		// will treat data as a single image and copy it into all layers
		void set_all_data(const void* data, u32 starting_layer = 0, u32 layer_count = 1) override;

		// will treat data as an array of images and copy each one into a separate layer
		void set_each_data(const void* data) override;

		void set_each_data_2(const void* datas[]) override;

		void copy_from_buffer(
			vk::Buffer buffer,
			u32 starting_layer = 0,
			u32 layer_count = 1
		);

		void copy_each_from_buffer(vk::Buffer buffer);

		void copy_from_buffers(
			const vk::Buffer* buffers,
			u32 buffer_count,
			u32 starting_layer = 0
		);

		[[nodiscard]] inline vk::Image& img(void) { return m_Image; }
		[[nodiscard]] inline const vk::Image& img(void) const { return m_Image; }

		[[nodiscard]] inline vk::DeviceMemory& memory(void) { return m_Memory; }
		[[nodiscard]] inline const vk::DeviceMemory& memory(void) const { return m_Memory; }

		[[nodiscard]] inline vk::ImageView& img_view(void) { return m_ImageView; }
		[[nodiscard]] inline const vk::ImageView& img_view(void) const { return m_ImageView; }
	private:
		vk::Image m_Image = nullptr;
		vk::DeviceMemory m_Memory = nullptr;

		vk::ImageView m_ImageView = nullptr;

		vk::ImageAspectFlags m_AspectMask;

		vk::ImageLayout m_CurrentLayout = vk::ImageLayout::eUndefined;
		vk::AccessFlags m_CurrentAccessMask = {};
	};
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_DEVICE_IMAGE_HPP