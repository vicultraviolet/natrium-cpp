#if !defined(NA_PIPELINE_HPP)
#define NA_PIPELINE_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VertexAttributes.hpp"
#include "Natrium/Graphics/Shader.hpp"
#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::Graphics {
	class RenderTarget;

	using Shaders = std::initializer_list<View<const Shader>>;
	using UniformLayout = std::initializer_list<View<const UniformSetLayout>>;

	enum class PipelineType : u8 {
		None = 0,
		Triangle, Compute,

		Graphics = Triangle
	};

	class Pipeline {
	public:
		virtual ~Pipeline(void) { this->destroy(); }
		virtual void destroy(void) {}

		[[nodiscard]] virtual PipelineType type(void) const = 0;

		[[nodiscard]] virtual operator bool(void) const = 0;
	};

	class TrianglePipeline : public Pipeline {
	public:
		[[nodiscard]] static UniqueRef<TrianglePipeline> Make(
			WeakRef<const RenderTarget> render_target,
			const VertexAttributes& vertex_layout,
			const View<const UniformSetLayout>* uniform_set_layouts,
			u64 uniform_set_layout_count,
			const View<const Shader>* shaders,
			u64 shader_count
		);

		[[nodiscard]] static UniqueRef<TrianglePipeline> Make(
			WeakRef<const RenderTarget> render_target,
			const VertexAttributes& vertex_layout = {},
			const UniformLayout& uniform_layout = {},
			const Shaders& shaders = {}
		);

		[[nodiscard]] inline PipelineType type(void) const override { return PipelineType::Triangle; }
	};
	using GraphicsPipeline = TrianglePipeline;

	class ComputePipeline : public Pipeline {
	public:
		[[nodiscard]] static UniqueRef<ComputePipeline> Make(
			View<const Shader> shader,
			const View<const UniformSetLayout>* uniform_set_layouts,
			u64 uniform_set_layout_count
		);

		[[nodiscard]] static UniqueRef<ComputePipeline> Make(
			View<const Shader> shader,
			const UniformLayout& uniform_layout = {}
		);

		[[nodiscard]] inline PipelineType type(void) const override { return PipelineType::Compute; }
	};
} // namespace Na::Graphics

#endif // NA_PIPELINE_HPP