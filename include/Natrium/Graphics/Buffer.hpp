#if !defined(NA_GRAPHICS_BUFFER_HPP)
#define NA_GRAPHICS_BUFFER_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VertexAttributes.hpp"
#include "Natrium/Assets/RendererSettingsAsset.hpp"

namespace Na::Graphics {
	enum class BufferTypeFlags : u8 {
		None = 0,

		VertexBuffer = NA_BIT(0),
		IndexBuffer = NA_BIT(1),
		StorageBuffer = NA_BIT(2),
		UniformBuffer = NA_BIT(3),

		All = VertexBuffer | IndexBuffer | StorageBuffer | UniformBuffer
	};

	struct BufferCreateInfo {
		u64 size = 0;
		u64 subbuffer_count = 1;
		bool cpu_accessible = true;
		BufferTypeFlags type = BufferTypeFlags::None;
	};

	class Buffer {
	public:
		static UniqueRef<Buffer> Make(const BufferCreateInfo& info);
		virtual ~Buffer(void) = default;

		virtual void set_data(const void* data) = 0;
		virtual void set_data_x(const void* data, u64 offset, u64 size) = 0;
		virtual void set_subdata(const void* data, u64 index) = 0;

		virtual Byte* map(void) = 0;
		virtual Byte* map_x(u64 offset, u64 size) = 0;

		virtual void unmap(void) = 0;

		[[nodiscard]] virtual Byte* mapped(void) const = 0;

		[[nodiscard]] BufferTypeFlags type(void) const { return m_Type; }
		[[nodiscard]] bool cpu_accessible(void) const { return m_CpuAccessible; }

		[[nodiscard]] virtual u64 element_size(void) const = 0;
		[[nodiscard]] virtual u64 aligned_size(void) const = 0;
		[[nodiscard]] virtual u64 total_size(void) const = 0;
		[[nodiscard]] virtual u64 subbuffer_count(void) const = 0;

		[[nodiscard]] inline bool is_multibuffer(void) const { return this->subbuffer_count() > 1; }

		[[nodiscard]] virtual operator bool(void) const = 0;
	protected:
		Buffer(void) = default;
		Buffer(const BufferCreateInfo& info);
	private:
		BufferTypeFlags m_Type = BufferTypeFlags::None;
		bool m_CpuAccessible = true;
	};

	UniqueRef<Buffer> MakeVertexBuffer(u64 size);
	UniqueRef<Buffer> MakeIndexBuffer(u32 count);
	UniqueRef<Buffer> MakeUniformBuffer(u64 size, u64 subbuffer_count);
	UniqueRef<Buffer> MakeStorageBuffer(u64 size, u64 subbuffer_count);

	inline BufferTypeFlags operator|(BufferTypeFlags lhs, BufferTypeFlags rhs) { return (BufferTypeFlags)((u8)lhs | (u8)rhs); }
	inline BufferTypeFlags operator&(BufferTypeFlags lhs, BufferTypeFlags rhs) { return (BufferTypeFlags)((u8)lhs & (u8)rhs); }
	inline BufferTypeFlags operator^(BufferTypeFlags lhs, BufferTypeFlags rhs) { return (BufferTypeFlags)((u8)lhs ^ (u8)rhs); }
	inline BufferTypeFlags operator~(BufferTypeFlags state) { return (BufferTypeFlags)(~(u8)state); }

	inline BufferTypeFlags& operator|=(BufferTypeFlags& lhs, BufferTypeFlags rhs) { lhs = lhs | rhs; return lhs; }
	inline BufferTypeFlags& operator&=(BufferTypeFlags& lhs, BufferTypeFlags rhs) { lhs = lhs & rhs; return lhs; }
	inline BufferTypeFlags& operator^=(BufferTypeFlags& lhs, BufferTypeFlags rhs) { lhs = lhs ^ rhs; return lhs; }

	inline bool operator==(BufferTypeFlags lhs, BufferTypeFlags rhs) { return (u8)lhs == (u8)rhs; }
	inline bool operator!=(BufferTypeFlags lhs, BufferTypeFlags rhs) { return (u8)lhs != (u8)rhs; }

	inline bool operator!(BufferTypeFlags state) { return (u8)state == 0; }
} // namespace Na::Graphics

#endif // NA_GRAPHICS_BUFFER_HPP