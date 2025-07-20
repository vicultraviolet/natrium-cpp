#if !defined(NA_VULKAN_IMPL_PIPELINE_HPP)
#define NA_VULKAN_IMPL_PIPELINE_HPP

#include "Natrium/Graphics/Pipeline.hpp"
#include "Natrium/Graphics/Renderer.hpp"
#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::VulkanImpl {
	class Pipeline : public Graphics::Pipeline {
	public:
		Pipeline(void) = default;
		Pipeline(
			View<const Graphics::Renderer> renderer,
			const Graphics::VertexAttributes& vertex_layout = {},
			const View<const Graphics::UniformSetLayout>* uniform_set_layouts = nullptr,
			u64 uniform_set_layout_count = 0,
			const View<const Graphics::Shader>* shaders = nullptr,
			u64 shader_count = 0
		);

		~Pipeline(void) { this->destroy(); }
		void destroy(void) override;

		Pipeline(const Pipeline& other) = delete;
		Pipeline& operator=(const Pipeline& other) = delete;

		Pipeline(Pipeline&& other);
		Pipeline& operator=(Pipeline&& other);

		[[nodiscard]] inline vk::Pipeline& pipeline(void) { return m_Pipeline; }
		[[nodiscard]] inline const vk::Pipeline& pipeline(void) const { return m_Pipeline; }

		[[nodiscard]] inline vk::PipelineLayout& layout(void) { return m_Layout; }
		[[nodiscard]] inline const vk::PipelineLayout& layout(void) const { return m_Layout; }

		[[nodiscard]] inline operator bool(void) const { return m_Pipeline; }
	private:
		vk::Pipeline m_Pipeline;
		vk::PipelineLayout m_Layout;
	};
} // namespace Na

#endif // NA_VULKAN_IMPL_PIPELINE_HPP