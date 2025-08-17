#if !defined(NA_VULKAN_IMPL_TRIANGLE_PIPELINE_HPP)
#define NA_VULKAN_IMPL_TRIANGLE_PIPELINE_HPP

#include "Natrium/Graphics/VulkanImpl/vPipeline.hpp"
#include "Natrium/Graphics/RenderTargets.hpp"
#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::VulkanImpl {
	class TrianglePipeline : public Graphics::TrianglePipeline {
	public:
		TrianglePipeline(
			WeakRef<const Graphics::RenderTarget> render_target,
			const Graphics::VertexAttributes& vertex_layout = {},
			const View<const Graphics::UniformSetLayout>* uniform_set_layouts = nullptr,
			u64 uniform_set_layout_count = 0,
			const View<const Graphics::Shader>* shaders = nullptr,
			u64 shader_count = 0
		);

		~TrianglePipeline(void) { this->destroy(); }
		void destroy(void) { m_Pipeline.destroy(); }

		[[nodiscard]] vk::Pipeline& pipeline(void) { return m_Pipeline.pipeline; }
		[[nodiscard]] const vk::Pipeline& pipeline(void) const { return m_Pipeline.pipeline; }

		[[nodiscard]] vk::PipelineLayout& layout(void) { return m_Pipeline.layout; }
		[[nodiscard]] const vk::PipelineLayout& layout(void) const { return m_Pipeline.layout; }

		[[nodiscard]] inline operator bool(void) const override { return m_Pipeline; }
	private:
		VulkanImpl::Pipeline m_Pipeline;
	};
} // namespace Na

#endif // NA_VULKAN_IMPL_TRIANGLE_PIPELINE_HPP