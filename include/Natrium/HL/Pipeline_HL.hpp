#if !defined(NA_HL_PIPELINE_HPP)
#define NA_HL_PIPELINE_HPP

#include "Natrium/Graphics/Pipelines.hpp"
#include "Natrium/HL/UniformManager_HL.hpp"

namespace Na::HL {
	using PipelineType = Graphics::PipelineType;

	struct TrianglePipelineCreateInfo {
		WeakRef<const Graphics::RenderTarget> render_target;

		Graphics::Shaders shaders;

		View<const Graphics::VertexAttributes> vertex_attributes;
		View<const UniformSetLayouts> uniform_set_layouts;
	};

	struct ComputePipelineCreateInfo {
		View<const Graphics::Shader> shader;
		View<const UniformSetLayouts> uniform_set_layouts;
	};

	class Pipeline {
	public:
		Pipeline(void) = default;

		~Pipeline(void) { this->destroy(); }
		void destroy(void) { m_Pipeline.destroy(); }

		Pipeline(const TrianglePipelineCreateInfo& info);
		Pipeline(const ComputePipelineCreateInfo& info);

		Pipeline(Pipeline&& other) noexcept;
		Pipeline& operator=(Pipeline&& other) noexcept;

		[[nodiscard]] inline View<const Graphics::Pipeline> native(void) const { return m_Pipeline; }
		[[nodiscard]] inline View<Graphics::Pipeline> native(void) { return m_Pipeline; }

		[[nodiscard]] inline PipelineType type(void) const { return m_Pipeline->type(); }
	private:
		UniqueRef<Graphics::Pipeline> m_Pipeline;
	};
} // namespace Na::HL

#endif // NA_HL_PIPELINE_HPP