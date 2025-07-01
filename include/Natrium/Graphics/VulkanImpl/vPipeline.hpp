#if !defined(NA_VULKAN_IMPL_PIPELINE_HPP)
#define NA_VULKAN_IMPL_PIPELINE_HPP

#include "Natrium/Graphics/Pipeline.hpp"
#include "Natrium/Graphics/Renderer.hpp"

namespace Na::VulkanImpl {
	using Shaders = std::initializer_list<View<const Graphics::Shader>>;

	class Pipeline : public Graphics::Pipeline {
	public:
		Pipeline(void) = default;
		Pipeline(
			View<const Graphics::Renderer> renderer,
			const Graphics::VertexAttributes& vertex_shader_layout = {},
			const Shaders& shaders = {}
		);

		~Pipeline(void) { this->destroy(); }
		void destroy(void) override;

		Pipeline(const Pipeline& other) = delete;
		Pipeline& operator=(const Pipeline& other) = delete;

		Pipeline(Pipeline&& other);
		Pipeline& operator=(Pipeline&& other);

		[[nodiscard]] inline vk::Pipeline pipeline(void) const { return m_Pipeline; }

		[[nodiscard]] inline vk::DescriptorSetLayout descriptor_layout(void) const { return m_DescriptorLayout; }
		[[nodiscard]] inline vk::PipelineLayout layout(void) const { return m_Layout; }

		[[nodiscard]] inline vk::DescriptorPool descriptor_pool(void) const { return m_DescriptorPool; }
		[[nodiscard]] inline const vk::DescriptorSet& descriptor_set(void) const { return m_DescriptorSet; }

		[[nodiscard]] inline ArrayList<u32>& dynamic_offsets(void) { return m_DynamicOffsets; }
		[[nodiscard]] inline const ArrayList<u32>& dynamic_offsets(void) const { return m_DynamicOffsets; }
		[[nodiscard]] inline u32 dynamic_offset_count(void) const { return m_DynamicOffsetCount; }
		[[nodiscard]] inline u32 dynamic_offset_index(void) const { return m_DynamicOffsetIndex; }
		inline void increment_dynamic_offset_index(void) { m_DynamicOffsetIndex++; }

		[[nodiscard]] inline operator bool(void) const { return m_Pipeline; }
	private:
		void _set_descriptor_set(const Shaders& shaders, u32 max_frames_in_flight);
	private:
		vk::Pipeline m_Pipeline;

		vk::DescriptorSetLayout m_DescriptorLayout;
		vk::PipelineLayout m_Layout;

		vk::DescriptorPool m_DescriptorPool;
		vk::DescriptorSet m_DescriptorSet;

		ArrayList<u32> m_DynamicOffsets;
		u32 m_DynamicOffsetCount = 0;
		u32 m_DynamicOffsetIndex = 0;
	};
} // namespace Na

#endif // NA_VULKAN_IMPL_PIPELINE_HPP