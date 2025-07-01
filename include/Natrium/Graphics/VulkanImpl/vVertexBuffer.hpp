#if !defined(NA_VK_IMPL_VERTEX_BUFFER_HPP)
#define NA_VK_IMPL_VERTEX_BUFFER_HPP

#include "Natrium/Graphics/Buffers.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceBuffer.hpp"

namespace Na::VulkanImpl {
	class VertexBuffer : public Graphics::VertexBuffer {
	public:
		VertexBuffer(void) = default;
		VertexBuffer(u64 size, const void* data = nullptr);

		~VertexBuffer(void) { this->destroy(); }
		void destroy(void) override;

		VertexBuffer(const VertexBuffer& other) = delete;
		VertexBuffer& operator=(const VertexBuffer& other) = delete;

		VertexBuffer(VertexBuffer&& other) noexcept;
		VertexBuffer& operator=(VertexBuffer&& other) noexcept;

		void set_data(const void* data) override;

		[[nodiscard]] inline u64 size(void) const override { return m_Buffer.size; }

		[[nodiscard]] inline const DeviceBuffer& buffer(void) const { return m_Buffer; }

		[[nodiscard]] inline vk::Buffer native(void) const { return m_Buffer.buffer; }

		[[nodiscard]] inline operator bool(void) const override { return m_Buffer; }
	private:
		DeviceBuffer m_Buffer;
	};
} // namespace Na

#endif // NA_VK_IMPL_VERTEX_BUFFER_HPP