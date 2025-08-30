#if !defined(NA_GRAPHICS_UNIFORM_SET_HPP)
#define NA_GRAPHICS_UNIFORM_SET_HPP

#include "Natrium/Graphics/UniformSetLayout.hpp"
#include "Natrium/Graphics/Texture.hpp"
#include "Natrium/Graphics/Buffer.hpp"

namespace Na::Graphics {
	class Renderer;

	struct UniformSetBufferBindingInfo;
	struct UniformSetTextureBindingInfo;

	struct UniformSetBufferBindingInfo2;
	struct UniformSetTextureBindingInfo2;

	class UniformSet {
	public:
		[[nodiscard]] static UniqueRef<UniformSet> Make(
			View<const UniformSetLayout> layout,
			View<const Renderer> renderer
		);
		virtual ~UniformSet(void) = default;

		virtual void bind(const UniformSetBufferBindingInfo& info) = 0;
		virtual void bind(const UniformSetTextureBindingInfo& info) = 0;

		virtual void bind_array(const UniformSetBufferBindingInfo2& info) = 0;
		virtual void bind_array(const UniformSetTextureBindingInfo2& info) = 0;

		[[nodiscard]] virtual operator bool(void) const = 0;
	};

	struct UniformSetBufferBindingInfo {
		u32 binding = u32max;
		u32 array_index = 0;

		View<const Graphics::Buffer> buffer;

		// buffer itself can be multiple types, but for binding we need to know which one
		BufferTypeFlags type = BufferTypeFlags::None;
	};

	struct UniformSetTextureBindingInfo {
		u32 binding = u32max;
		u32 array_index = 0;

		View<const Graphics::Texture> texture;
	};

	struct UniformSetBufferBindingInfo2 {
		u32 binding = u32max;
		u32 starting_index = 0;

		const View<const Graphics::Buffer>* buffers = nullptr;
		u32 buffer_count = 0;

		// buffer itself can be multiple types, but for binding we need to know which one
		BufferTypeFlags type = BufferTypeFlags::None;
	};

	struct UniformSetTextureBindingInfo2 {
		u32 binding = u32max;
		u32 starting_index = 0;

		const View<const Graphics::Texture>* textures = nullptr;
		u32 texture_count = 0;
	};
} // namespace Na::Graphics

#endif // NA_GRAPHICS_UNIFORM_SET_HPP