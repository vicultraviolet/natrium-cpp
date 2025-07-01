#if !defined(NA_GRAPHICS_BUFFERS_HPP)
#define NA_GRAPHICS_BUFFERS_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VertexAttributes.hpp"

namespace Na::Graphics {
	class VertexBuffer {
	public:
		static UniqueRef<VertexBuffer> Make(u64 size, const void* data = nullptr);
		virtual ~VertexBuffer(void) { this->destroy(); }

		virtual void destroy(void) {}

		virtual void set_data(const void* data) = 0;

		[[nodiscard]] virtual u64 size(void) const = 0;
		[[nodiscard]] virtual operator bool(void) const = 0;
	};

	class IndexBuffer {
	public:
		static UniqueRef<IndexBuffer> Make(u32 count, const u32* data = nullptr);
		virtual ~IndexBuffer(void) { this->destroy(); }

		virtual void destroy(void) {}

		virtual void set_data(const u32* data) = 0;

		[[nodiscard]] virtual u32 count(void) const = 0;
		[[nodiscard]] virtual u64 size(void) const = 0;
		[[nodiscard]] virtual u64 index_size(void) const = 0;
		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_GRAPHICS_BUFFERS_HPP