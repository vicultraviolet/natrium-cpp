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

	class Pipeline {
	public:
		[[nodiscard]] static UniqueRef<Pipeline> Make(
			View<const Renderer> renderer,
			const VertexAttributes& vertex_layout = {},
			const View<const UniformSetLayout>* uniform_set_layouts = nullptr,
			u64 uniform_set_layout_count = 0,
			const View<const Shader>* shaders = nullptr,
			u64 shader_count = 0
		);

		[[nodiscard]] static UniqueRef<Pipeline> Make(
			View<const Renderer> renderer,
			const VertexAttributes& vertex_layout = {},
			const UniformLayout& uniform_layout = {},
			const Shaders& shaders = {}
		);

		virtual ~Pipeline(void) { this->destroy(); }
		virtual void destroy(void) {}

		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_PIPELINE_HPP