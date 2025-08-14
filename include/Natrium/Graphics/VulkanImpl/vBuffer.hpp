#if !defined(NA_VULKAN_IMPL_BUFFER_HPP)
#define NA_VULKAN_IMPL_BUFFER_HPP

#include "Natrium/Graphics/Buffer.hpp"

namespace Na::VulkanImpl {
	struct BufferCreateInfo2 {
		vk::DeviceSize size = 0;
		vk::DeviceSize count = 1;
		vk::BufferUsageFlags usage;
		vk::MemoryPropertyFlags memory_props;
		vk::SharingMode sharing_mode = vk::SharingMode::eExclusive;
	};

	using BufferCreateInfo = Graphics::BufferCreateInfo;
	using BufferTypeFlags = Graphics::BufferTypeFlags;

	vk::BufferUsageFlags BufferTypeToVk(BufferTypeFlags type);

	u32 FindMemoryType(u32 type_filter, vk::MemoryPropertyFlags props);

	class Buffer : public Graphics::Buffer {
	public:
		Buffer(const BufferCreateInfo& info);
		Buffer(const BufferCreateInfo2& info);

		~Buffer(void) { this->destroy(); }
		void destroy(void);

		void set_data(const void* data) override;
		void set_data_x(const void* data, u64 offset, u64 size) override;
		void set_subdata(const void* data, u64 index) override;

		Byte* map(void) override;
		Byte* map_x(u64 offset, u64 size) override;

		void unmap(void) override;

		[[nodiscard]] inline Byte* mapped(void) const override { return m_Mapped; }

		void copy(const Buffer& other);
		void copy_x(const Buffer& other, u64 src_offset, u64 dst_offset, u64 size);

		[[nodiscard]] inline u64 element_size(void) const override { return m_ElementSize; }
		[[nodiscard]] inline u64 aligned_size(void) const override { return m_AlignedSize; }
		[[nodiscard]] inline u64 total_size(void) const override { return m_TotalSize; }
		[[nodiscard]] inline u64 subbuffer_count(void) const override { return m_SubBufferCount; }

		[[nodiscard]] inline operator bool(void) const override { return m_Buffer; }

		[[nodiscard]] inline vk::Buffer& native(void) { return m_Buffer; }
		[[nodiscard]] inline const vk::Buffer& native(void) const { return m_Buffer; }

		[[nodiscard]] inline vk::DeviceMemory& memory(void) { return m_Memory; }
		[[nodiscard]] inline const vk::DeviceMemory& memory(void) const { return m_Memory; }
	private:
		vk::Buffer m_Buffer = nullptr;
		vk::DeviceMemory m_Memory = nullptr;

		Byte* m_Mapped = nullptr;

		u64 m_ElementSize = 0;
		u64 m_AlignedSize = 0;
		u64 m_TotalSize = 0;
		u64 m_SubBufferCount = 0;
	};
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_BUFFER_HPP