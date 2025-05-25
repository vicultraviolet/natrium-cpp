#if !defined(NA_STORAGE_BUFFER_HPP)
#define NA_STORAGE_BUFFER_HPP

#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

namespace Na {
	class StorageBuffer {
	public:
		const ShaderUniformType descriptor_type = ShaderUniformType::StorageBuffer;

		StorageBuffer(void) = default;
		StorageBuffer(u64 size, AssetHandle<RendererSettings> renderer_settings);
		void destroy(void);
		inline ~StorageBuffer(void) { this->destroy(); }

		StorageBuffer(const StorageBuffer& other) = delete;
		StorageBuffer& operator=(const StorageBuffer& other) = delete;

		StorageBuffer(StorageBuffer&& other);
		StorageBuffer& operator=(StorageBuffer&& other);

		[[nodiscard]] inline u64 size(void) const { return m_PerFrameSize; }
		[[nodiscard]] inline u64 per_frame_size(void) const { return m_PerFrameSize; }
		[[nodiscard]] inline u64 aligned_size(void) const { return m_AlignedSize; }
		[[nodiscard]] inline u64 total_size(void) const { return m_Buffer.size; }

		[[nodiscard]] inline operator bool(void) const { return m_Buffer; }

		[[nodiscard]] inline const DeviceBuffer& buffer(void) const { return m_Buffer; }
		[[nodiscard]] inline void* mapped_data(void) const { return m_Mapped; }
	private:
		DeviceBuffer m_Buffer;
		void* m_Mapped;

		u64 m_PerFrameSize = 0;
		u64 m_AlignedSize = 0;
	};
} // namespace Na

#endif // NA_STORAGE_BUFFER_HPP