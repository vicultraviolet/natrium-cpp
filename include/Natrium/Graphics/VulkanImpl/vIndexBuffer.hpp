#if !defined(NA_INDEX_BUFFER_HPP)
#define NA_INDEX_BUFFER_HPP

#include "Natrium/Graphics/Buffers.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceBuffer.hpp"

namespace Na::VulkanImpl {
	class IndexBuffer : public Graphics::IndexBuffer {
	public:
		IndexBuffer(void) = default;
		IndexBuffer(u32 count, const u32* data = nullptr);

		~IndexBuffer(void) { this->destroy(); }
		void destroy(void) override;

		IndexBuffer(const IndexBuffer& other) = delete;
		IndexBuffer& operator=(const IndexBuffer& other) = delete;

		IndexBuffer(IndexBuffer&& other) noexcept;
		IndexBuffer& operator=(IndexBuffer&& other) noexcept;

		void set_data(const u32* data) override;

		[[nodiscard]] inline u32 count(void) const override { return m_Count; }
		[[nodiscard]] inline u64 size(void) const override { return m_Buffer.size; }
		[[nodiscard]] inline u64 index_size(void) const override { return sizeof(u32); }
		[[nodiscard]] inline operator bool(void) const override { return m_Count; }

		[[nodiscard]] inline const DeviceBuffer& buffer(void) const { return m_Buffer; }

		[[nodiscard]] inline vk::Buffer native(void) const { return m_Buffer.buffer; }
	private:
		DeviceBuffer m_Buffer;
		u32 m_Count = 0;
	};
} // namespace Na

#endif // NA_INDEX_BUFFER_HPP