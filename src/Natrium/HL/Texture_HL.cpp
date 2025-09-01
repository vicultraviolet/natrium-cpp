#include "Pch.hpp"
#include "Natrium/HL/Texture_HL.hpp"

namespace Na::HL {
	Texture::Texture(
		WeakRef<RendererSettingsAsset> renderer_settings,
		u32 width,
		u32 height,
		u32 layer_count
	)
	{
		Graphics::DeviceImageCreateInfo image_info
		{
			.width = width,
			.height = height,
			.layer_count = layer_count,

			.format = Graphics::ImageFormat::Rgba8,

			.type = Graphics::DeviceImageTypeFlags::Sampled
		};

		m_Image = Graphics::DeviceImage::Make(image_info);

		Graphics::SamplerCreateInfo sampler_info
		{
			.oversampling_filter = Graphics::SamplerFilter::Linear,
			.undersampling_filter = Graphics::SamplerFilter::Linear,

			.adress_mode_u = Graphics::SamplerAddressMode::Repeat,
			.adress_mode_v = Graphics::SamplerAddressMode::Repeat,

			.renderer_settings = renderer_settings
		};

		m_Sampler = Graphics::Sampler::Make(sampler_info);
	}

	Texture::Texture(
		const Graphics::DeviceImageCreateInfo& image_info,
		const Graphics::SamplerCreateInfo& sampler_info
	)
	{
		m_Image = Graphics::DeviceImage::Make(image_info);
		m_Sampler = Graphics::Sampler::Make(sampler_info);
	}

	void Texture::set_data(WeakRef<const HostImage> _img)
	{
		auto img = _img.lock();

		m_Image->set_all_data(img->data());
	}

	void Texture::set_data(const WeakRef<const HostImage> imgs[])
	{
		ArrayList<void*> datas(m_Image->layer_count(), m_Image->layer_count());

		for (u32 i = 0; i < m_Image->layer_count(); i++)
		{
			datas[i] = imgs[i].lock()->data();
		}

		m_Image->set_each_data_2((const void*[])datas.ptr());
	}
} // namespace Na::HL
