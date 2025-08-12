#if !defined(NA_VULKAN_IMPL_COMPUTE_PIPELINE_HPP)
#define NA_VULKAN_IMPL_COMPUTE_PIPELINE_HPP

#include "Natrium/Graphics/VulkanImpl/vPipeline.hpp"
#include "Natrium/Graphics/Renderer.hpp"
#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::VulkanImpl {
	class ComputePipeline : public Graphics::ComputePipeline {
	public:
		ComputePipeline(
			View<const Graphics::Shader> shader,
			const View<const Graphics::UniformSetLayout>* uniform_set_layouts = nullptr,
			u64 uniform_set_layout_count = 0
		);

		~ComputePipeline(void) { this->destroy(); }
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

#endif // NA_VULKAN_IMPL_COMPUTE_PIPELINE_HPP