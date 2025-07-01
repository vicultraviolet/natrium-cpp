#if !defined(NA_VULKAN_IMPL_UNIFORM_BUFFER_HPP)
#define NA_VULKAN_IMPL_UNIFORM_BUFFER_HPP

#include "Natrium/Graphics/Uniforms.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceBuffer.hpp"

namespace Na::VulkanImpl {
	class UniformBuffer : public Graphics::UniformBuffer {
	public:
		UniformBuffer(void) = default;
		UniformBuffer(u64 size, Ref<const RendererSettingsAsset> renderer_settings);

		~UniformBuffer(void) { this->destroy(); }
		void destroy(void) override;

		UniformBuffer(const UniformBuffer& other) = delete;
		UniformBuffer& operator=(const UniformBuffer& other) = delete;

		UniformBuffer(UniformBuffer&& other) noexcept;
		UniformBuffer& operator=(UniformBuffer&& other) noexcept;

		[[nodiscard]] inline u64 size(void) const override { return m_PerFrameSize; }

		[[nodiscard]] inline u64 per_frame_size(void) const { return m_PerFrameSize; }
		[[nodiscard]] inline u64 aligned_size(void) const { return m_AlignedSize; }
		[[nodiscard]] inline u64 total_size(void) const { return m_Buffer.size; }

		[[nodiscard]] inline operator bool(void) const override { return m_Buffer; }

		[[nodiscard]] inline const DeviceBuffer& buffer(void) const { return m_Buffer; }
		[[nodiscard]] inline void* mapped_data(void) const { return m_Mapped; }
	private:
		DeviceBuffer m_Buffer;
		void* m_Mapped;

		u64 m_PerFrameSize = 0;
		u64 m_AlignedSize = 0;
	};
} // namespace Na

#endif // NA_VULKAN_IMPL_UNIFORM_BUFFER_HPP