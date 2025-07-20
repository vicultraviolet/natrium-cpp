#if !defined(NA_GRAPHICS_UNIFORM_SET_HPP)
#define NA_GRAPHICS_UNIFORM_SET_HPP

#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::Graphics {
	class Renderer;

	class UniformSet {
	public:
		[[nodiscard]] static UniqueRef<UniformSet> Make(
			View<const UniformSetLayout> layout,
			View<const Renderer> renderer
		);

		virtual void destroy(void) {}
		virtual ~UniformSet(void) { this->destroy(); }

		virtual void bind_at(u32 binding, View<const Uniform> uniform) = 0;

		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_GRAPHICS_UNIFORM_SET_HPP