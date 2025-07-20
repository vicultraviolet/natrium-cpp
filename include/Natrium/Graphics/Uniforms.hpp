#if !defined(NA_GRAPHICS_UNIFORMS_HPP)
#define NA_GRAPHICS_UNIFORMS_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Assets/RendererSettingsAsset.hpp"
#include "Natrium/Assets/ImageAsset.hpp"

namespace Na::Graphics {
	enum class UniformType : u8 {
		None = 0,
		UniformBuffer, StorageBuffer, Texture
	};

	class Uniform {
	public:
		virtual ~Uniform(void) { this->destroy(); }
		virtual void destroy(void) {}

		[[nodiscard]] virtual UniformType type(void) const = 0;
		[[nodiscard]] virtual operator bool(void) const = 0;
	};

	class UniformBuffer : public Uniform {
	public:
		static UniqueRef<UniformBuffer> Make(u64 size, Ref<const RendererSettingsAsset> renderer_settings);

		[[nodiscard]] virtual u64 size(void) const = 0;
		[[nodiscard]] inline UniformType type(void) const override { return UniformType::UniformBuffer; }
	};

	class StorageBuffer : public Uniform {
	public:
		static UniqueRef<StorageBuffer> Make(u64 size, Ref<const RendererSettingsAsset> renderer_settings);

		[[nodiscard]] virtual u64 size(void) const = 0;
		[[nodiscard]] inline UniformType type(void) const override { return UniformType::StorageBuffer; }
	};

	class Texture : public Uniform {
	public:
		static UniqueRef<Texture> Make(
			const Ref<const ImageAsset>* imgs,
			u32 count,
			Ref<const RendererSettingsAsset> renderer_settings
		);

		static inline UniqueRef<Texture> Make(
			Ref<const ImageAsset> img,
			Ref<const RendererSettingsAsset> renderer_settings
		)
		{
			return Texture::Make(&img, 1, renderer_settings);
		}

		static inline UniqueRef<Texture> Make(
			const std::initializer_list<Ref<const ImageAsset>>& imgs,
			Ref<const RendererSettingsAsset> renderer_settings
		)
		{
			return Texture::Make(imgs.begin(), (u32)imgs.size(), renderer_settings);
		}

		[[nodiscard]] virtual u32 width(void) const = 0;
		[[nodiscard]] virtual u32 height(void) const = 0;
		[[nodiscard]] virtual u32 count(void) const = 0;

		[[nodiscard]] inline UniformType type(void) const override { return UniformType::Texture; }
	};
} // namespace Na::Graphics

#endif // NA_GRAPHICS_UNIFORMS_HPP