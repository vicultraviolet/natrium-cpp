#if !defined(NA_PIPELINE_HPP)
#define NA_PIPELINE_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VertexAttributes.hpp"
#include "Natrium/Graphics/Shader.hpp"
#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::Graphics {
	class Renderer;

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
			View<const Renderer> renderer,
			const VertexAttributes& vertex_layout = {},
			const View<const UniformSetLayout>* uniform_set_layouts = nullptr,
			u64 uniform_set_layout_count = 0,
			const View<const Shader>* shaders = nullptr,
			u64 shader_count = 0
		);

		[[nodiscard]] static UniqueRef<TrianglePipeline> Make(
			View<const Renderer> renderer,
			const VertexAttributes& vertex_layout = {},
			const UniformLayout& uniform_layout = {},
			const Shaders& shaders = {}
		);

		[[nodiscard]] inline PipelineType type(void) const override { return PipelineType::Triangle; }
	};
	using GraphicsPipeline = TrianglePipeline;

	class ComputePipeline : public Pipeline {
	public:


		[[nodiscard]] inline PipelineType type(void) const override { return PipelineType::Compute; }
	};
} // namespace Na::Graphics

#endif // NA_PIPELINE_HPP