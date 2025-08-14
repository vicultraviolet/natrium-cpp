#if !defined(NA_DEVICE_BUFFER)
#define NA_DEVICE_BUFFER

#include "Natrium/Core.hpp"
#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"

namespace Na::VulkanImpl {
	class DeviceBuffer {
	public:
		vk::Buffer buffer = nullptr;
		vk::DeviceSize size = 0;
		vk::DeviceMemory memory = nullptr;

		DeviceBuffer(void) = default;
		~DeviceBuffer(void) { this->destroy(); }

		DeviceBuffer(
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			vk::SharingMode sharing_mode = vk::SharingMode::eExclusive
		);

		void destroy(void);

		DeviceBuffer(const DeviceBuffer& other) = delete;
		DeviceBuffer& operator=(const DeviceBuffer& other) = delete;

		DeviceBuffer(DeviceBuffer&& other);
		DeviceBuffer& operator=(DeviceBuffer&& other);

		void copy(const DeviceBuffer& other);

		[[nodiscard]] inline operator bool(void) const { return buffer && size && memory; }
	};
} // namespace Na

#endif // NA_DEVICE_BUFFER