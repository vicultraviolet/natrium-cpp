#if !defined(NA_PIPELINE_HPP)
#define NA_PIPELINE_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VertexAttributes.hpp"
#include "Natrium/Graphics/Shader.hpp"

namespace Na::Graphics {
	class Renderer;

	class Pipeline {
	public:
		[[nodiscard]] static UniqueRef<Pipeline> Make(
			View<const Renderer> renderer,
			const VertexAttributes& vertex_shader_layout,
			const std::initializer_list<View<const Shader>>& shaders
		);

		virtual ~Pipeline(void) { this->destroy(); }
		virtual void destroy(void) {}

		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_PIPELINE_HPP