#if !defined(NA_GRAPHICS_UNIFORM_SET_LAYOUT_HPP)
#define NA_GRAPHICS_UNIFORM_SET_LAYOUT_HPP

#include "Natrium/Graphics/Uniforms.hpp"
#include "Natrium/Graphics/Shader.hpp"

namespace Na::Graphics {
	struct UniformBinding {
		u32 binding;
		UniformType type;
		ShaderStage shader_stage;
	};

	class UniformSetLayout {
	public:
		[[nodiscard]] static UniqueRef<UniformSetLayout> Make(
			const UniformBinding* bindings,
			u64 binding_count
		);

		[[nodiscard]] static UniqueRef<UniformSetLayout> Make(
			const std::initializer_list<UniformBinding>& bindings
		);

		virtual void destroy(void) {}
		virtual ~UniformSetLayout(void) { this->destroy(); }

		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_GRAPHICS_UNIFORM_SET_LAYOUT_HPP