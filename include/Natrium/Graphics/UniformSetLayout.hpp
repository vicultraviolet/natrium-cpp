#if !defined(NA_GRAPHICS_UNIFORM_SET_LAYOUT_HPP)
#define NA_GRAPHICS_UNIFORM_SET_LAYOUT_HPP

#include "Natrium/Graphics/Shader.hpp"

namespace Na::Graphics {
	enum class UniformType : u8 {
		None = 0,
		UniformBuffer, UniformMultibuffer,
		StorageBuffer, StorageMultibuffer,
		// StorageImage,
		Texture
	};

	struct UniformBinding {
		u32 binding;
		UniformType type;
		ShaderStage shader_stage;

		// arrays require the UniformIndexing extension
		u32 count = 1; 

		// requires the UniformIndexing extension with binding_partially_bound = true
		bool partially_bound = false;

		// requires the UniformIndexing extension with dynamic_count = true
		bool dynamic_count = false;
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