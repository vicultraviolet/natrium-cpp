#if !defined(NA_VULKAN_IMPL_TEXTURE_HPP)
#define NA_VULKAN_IMPL_TEXTURE_HPP

#include "Natrium/Graphics/Texture.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceImage.hpp"

namespace Na::VulkanImpl {
	class Texture : public Graphics::Texture {
	public:
		Texture(void) = default;
		Texture(
			const Ref<const ImageAsset>* imgs,
			u32 count,
			Ref<const RendererSettingsAsset> renderer_settings
		);

		~Texture(void) { this->destroy(); }
		void destroy(void);

		[[nodiscard]] inline operator bool(void) const override { return m_Image; }

		[[nodiscard]] inline u32 width(void) const override { return m_Image.width; }
		[[nodiscard]] inline u32 height(void) const override { return m_Image.height; }
		[[nodiscard]] inline u32 count(void) const override { return m_Image.layer_count(); }

		[[nodiscard]] inline const DeviceImage& img(void) const { return m_Image; }
		[[nodiscard]] inline vk::ImageView img_view(void) const { return m_ImageView; }
		[[nodiscard]] inline vk::Sampler sampler(void) const { return m_Sampler; }
	private:
		DeviceImage m_Image;
		vk::ImageView m_ImageView = nullptr;
		vk::Sampler m_Sampler = nullptr;
	};
} // namespace Na

#endif // NA_VULKAN_IMPL_TEXTURE_HPP