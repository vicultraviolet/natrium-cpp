#if !defined(NA_DEVICE_IMAGE_HPP)
#define NA_DEVICE_IMAGE_HPP

#include "Natrium/Core.hpp"

namespace Na::VulkanImpl {
	vk::Format FindSupportedFormat(
		const std::initializer_list<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	);

	class DeviceImage {
	public:
		vk::Image img = nullptr;
		vk::DeviceMemory memory = nullptr;

		union {
			vk::Extent3D extent;
			struct {
				u32 width, height, depth;
			};
		};
		vk::Format format = vk::Format::eUndefined;
		vk::ImageSubresourceRange subresource_range;

		DeviceImage(void) {};
		DeviceImage(
			const vk::Extent3D& extent,
			u32 layer_count,
			vk::ImageAspectFlagBits aspect_mask,
			vk::Format format,
			vk::ImageTiling tiling,
			vk::ImageUsageFlags usage,
			vk::SharingMode sharing_mode,
			vk::SampleCountFlagBits sample_count,
			vk::MemoryPropertyFlags memory_properties
		);
		void destroy(void);

		DeviceImage(const DeviceImage& other) = delete;
		DeviceImage& operator=(const DeviceImage& other) = delete;

		DeviceImage(DeviceImage&& other);
		DeviceImage& operator=(DeviceImage&& other);

		void transition_layout(vk::ImageLayout old_layout, vk::ImageLayout new_layout);

		void copy_from_buffer(vk::Buffer buffer, u32 starting_layer = 0, u32 layer_count = 1);

		void copy_all_from_buffer(vk::Buffer buffer, u32 starting_layer = 0);

		/// 
		/// copies each buffer into a separate layer, starting at starting_layer
		/// 
		void copy_from_buffers(const vk::Buffer* buffers, u32 buffer_count, u32 starting_layer = 0);

		/// 
		/// copies each buffer into a separate layer, starting at starting_layer
		/// 
		inline void copy_from_buffers(const std::initializer_list<vk::Buffer> buffers, u32 starting_layer = 0) { this->copy_from_buffers(buffers.begin(), (u32)buffers.size(), starting_layer); }

		[[nodiscard]] vk::ImageView create_img_view(void) const;

		[[nodiscard]] inline u32 layer_count(void) const { return this->subresource_range.layerCount; }
		[[nodiscard]] inline operator bool(void) const { return format != vk::Format::eUndefined; }
	};

	vk::ImageView CreateImageView(vk::Image img, vk::ImageAspectFlags aspect_mask, vk::Format format, u32 layer_count = 1);
}

#endif // NA_DEVICE_IMAGE_HPP