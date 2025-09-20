#if !defined(NA_HL_TEXTURE_HPP)
#define NA_HL_TEXTURE_HPP

#include "Natrium/Graphics/DeviceImage.hpp"
#include "Natrium/Graphics/Sampler.hpp"

#include "Natrium/Assets/HostImage.hpp"

#include "Natrium/Graphics/UniformSet.hpp"

namespace Na::HL {
	class Texture {
	public:
		Texture(void) = default;

		Texture(
			WeakRef<const RendererSettingsAsset> renderer_settings,
			u32 width,
			u32 height,
			u32 layer_count = 1
		);

		Texture(
			const Graphics::DeviceImageCreateInfo& image_info,
			const Graphics::SamplerCreateInfo& sampler_info
		);

		void set_data(WeakRef<const HostImage> img);
		void set_data(const WeakRef<const HostImage> imgs[]);

		[[nodiscard]] inline View<Graphics::DeviceImage> img(void) { return m_Image; }
		[[nodiscard]] inline View<const Graphics::DeviceImage> img(void) const { return m_Image; }

		[[nodiscard]] inline View<Graphics::Sampler> sampler(void) { return m_Sampler; }
		[[nodiscard]] inline View<const Graphics::Sampler> sampler(void) const { return m_Sampler; }

		[[nodiscard]] inline operator Graphics::UniformSetTextureInfo(void) const { return { m_Image, m_Sampler }; }
	private:
		void _set_data(const void* data, bool is_array);
	private:
		UniqueRef<Graphics::DeviceImage> m_Image;
		UniqueRef<Graphics::Sampler> m_Sampler;
	};
} // namespace Na::HL

#endif // NA_HL_TEXTURE_HPP