#if !defined(NA_TEXTURE_HPP)
#define NA_TEXTURE_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Assets/RendererSettingsAsset.hpp"
#include "Natrium/Assets/ImageAsset.hpp"
#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::Graphics {
	class Texture {
	public:
		[[nodiscard]] static UniqueRef<Texture> Make(
			const Ref<const ImageAsset>* imgs,
			u32 count,
			Ref<const RendererSettingsAsset> renderer_settings
		);

		[[nodiscard]] static UniqueRef<Texture> Make(
			Ref<const ImageAsset> img,
			Ref<const RendererSettingsAsset> renderer_settings
		);

		[[nodiscard]] static UniqueRef<Texture> Make(
			const std::initializer_list<Ref<const ImageAsset>>& imgs,
			Ref<const RendererSettingsAsset> renderer_settings
		);

		virtual ~Texture(void) = default;

		[[nodiscard]] virtual u32 width(void) const = 0;
		[[nodiscard]] virtual u32 height(void) const = 0;
		[[nodiscard]] virtual u32 count(void) const = 0;

		[[nodiscard]] virtual operator bool(void) const = 0;

		[[nodiscard]] inline UniformType type(void) const { return UniformType::Texture; }
	};
} // namespace Na::Graphics

#endif // NA_TEXTURE_HPP