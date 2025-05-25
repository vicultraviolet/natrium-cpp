#if !defined(NA_TEXTURE_HPP)
#define NA_TEXTURE_HPP

#include "Natrium/Assets/ImageAsset.hpp"
#include "Natrium/Graphics/DeviceImage.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

namespace Na {
	class Texture {
	public:
		const ShaderUniformType descriptor_type = ShaderUniformType::Texture;

		Texture(void) = default;
		Texture(AssetHandle<Image> img, AssetHandle<RendererSettings> renderer_settings)
		: Texture(&img, 1, renderer_settings) {}

		Texture(const AssetHandle<Image>* imgs, u32 count, AssetHandle<RendererSettings> renderer_settings);

		Texture(const std::initializer_list<AssetHandle<Image>>& imgs, AssetHandle<RendererSettings> renderer_settings)
		: Texture(imgs.begin(), (u32)imgs.size(), renderer_settings) {}

		inline ~Texture(void) { this->destroy(); }
		void destroy(void);

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;

		Texture(Texture&& other);
		Texture& operator=(Texture&& other);

		[[nodiscard]] inline bool is_array(void) const { return m_Image.layer_count() > 1; }

		[[nodiscard]] inline operator bool(void) const { return m_Image; }

		[[nodiscard]] inline u32 width(void) const { return m_Image.width; }
		[[nodiscard]] inline u32 height(void) const { return m_Image.height; }
		[[nodiscard]] inline u32 count(void) const { return m_Image.layer_count(); }

		[[nodiscard]] inline const DeviceImage& img(void) const { return m_Image; }
		[[nodiscard]] inline vk::ImageView img_view(void) const { return m_ImageView; }
		[[nodiscard]] inline vk::Sampler sampler(void) const { return m_Sampler; }
	private:
		DeviceImage m_Image;
		vk::ImageView m_ImageView = nullptr;
		vk::Sampler m_Sampler = nullptr;
	};
} // namespace Na

#endif // NA_TEXTURE_HPP