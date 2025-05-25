#if !defined(NA_UNIFORM_BUFFER_HPP)
#define NA_UNIFORM_BUFFER_HPP

#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

namespace Na {
	class UniformBuffer {
	public:
		const ShaderUniformType descriptor_type = ShaderUniformType::UniformBuffer;

		UniformBuffer(void) = default;
		UniformBuffer(u64 size, AssetHandle<RendererSettings> renderer_settings);
		void destroy(void);
		inline ~UniformBuffer(void) { this->destroy(); }

		UniformBuffer(const UniformBuffer& other) = delete;
		UniformBuffer& operator=(const UniformBuffer& other) = delete;

		UniformBuffer(UniformBuffer&& other);
		UniformBuffer& operator=(UniformBuffer&& other);

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

#endif // NA_UNIFORM_BUFFER_HPP