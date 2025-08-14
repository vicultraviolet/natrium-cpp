#if !defined(NA_GRAPHICS_UNIFORM_SET_HPP)
#define NA_GRAPHICS_UNIFORM_SET_HPP

#include "Natrium/Graphics/UniformSetLayout.hpp"
#include "Natrium/Graphics/Texture.hpp"
#include "Natrium/Graphics/Buffer.hpp"

namespace Na::Graphics {
	class Renderer;

	class UniformSet {
	public:
		[[nodiscard]] static UniqueRef<UniformSet> Make(
			View<const UniformSetLayout> layout,
			View<const Renderer> renderer
		);
		virtual ~UniformSet(void) = default;

		virtual void bind_at(u32 binding, View<const Buffer> buffer, BufferTypeFlags type) = 0;
		virtual void bind_at(u32 binding, View<const Texture> texture) = 0;

		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_GRAPHICS_UNIFORM_SET_HPP