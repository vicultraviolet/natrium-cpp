#include "Pch.hpp"
#include "Natrium/HL/Texture_HL.hpp"

#include <stb/stb_image_write.h>

namespace Na::HL {
	Texture::Texture(
		const UUID_t& uuid,
		TextureDimensions dimensions,
		WeakRef<const RendererSettings> renderer_settings,
		Graphics::SamplerFilter filter,
		bool saveable
	)
	: Asset(uuid)
	{
		Graphics::DeviceImageCreateInfo image_info
		{
			.dimensions = std::move(dimensions),

			.format = Graphics::ImageFormat::Rgba8,
			.type = Graphics::DeviceImageTypeFlags::Sampled | Graphics::DeviceImageTypeFlags::TransferDst
		};
		if (saveable)
			image_info.type |= Graphics::DeviceImageTypeFlags::TransferSrc;

		m_Image = Graphics::DeviceImage::Make(image_info);

		Graphics::SamplerCreateInfo sampler_info
		{
			.oversampling_filter = filter,
			.undersampling_filter = filter,

			.adress_mode_u = Graphics::SamplerAddressMode::Repeat,
			.adress_mode_v = Graphics::SamplerAddressMode::Repeat,

			.renderer_settings = renderer_settings
		};

		m_Sampler = Graphics::Sampler::Make(sampler_info);
	}

	Texture::Texture(
		const UUID_t& uuid,
		const Graphics::DeviceImageCreateInfo& image_info,
		const Graphics::SamplerCreateInfo& sampler_info
	) : Asset(uuid)
	{
		m_Image = Graphics::DeviceImage::Make(image_info);
		m_Sampler = Graphics::Sampler::Make(sampler_info);
	}

	FileErrorCode Texture::load(const std::filesystem::path& path)
	{
		auto img = MakeRef<HostImage>();

		FileErrorCode code = img->load(path);
		if (code != FileErrorCode::None)
		{
			return code;
		}

		this->set_data(img);

		return FileErrorCode::None;
	}

	FileErrorCode Texture::save(const std::filesystem::path& path)
	{
		Na::Graphics::DeviceImageBarrierInfo barrier_info;

		barrier_info = {
			.new_img_state = Na::Graphics::DeviceImageState::TransferSrc,

			.before = {
				Na::Graphics::BarrierStageBits::Transfer | Na::Graphics::BarrierStageBits::ComputeShader,
				Na::Graphics::BarrierOperationBits::TransferWrite | Na::Graphics::BarrierOperationBits::ShaderWrite
			},

			.after = {
				Na::Graphics::BarrierStageBits::Transfer,
				Na::Graphics::BarrierOperationBits::TransferRead
			}
		};
		m_Image->barrier(barrier_info);

		auto buffer = m_Image->copy_to_buffer();

		barrier_info = {
			.new_img_state = Graphics::DeviceImageState::Texture,

			.before = {
				Graphics::BarrierStageBits::Transfer,
				Graphics::BarrierOperationBits::TransferRead
			},

			.after = {
				Graphics::BarrierStageBits::FragmentShader,
				Graphics::BarrierOperationBits::ShaderRead
			}
		};
		m_Image->barrier(barrier_info);

		auto path_str = path.string();

		if (path.extension() == ".png")
		{
			stbi_write_png(
				path_str.c_str(),
				(int)m_Image->dimensions().w, (int)m_Image->dimensions().h,
				Graphics::ImageFormat_GetChannelCount(m_Image->format()),
				buffer->map(),
				m_Image->stride()
			);
		} else
		if (path.extension() == ".jpeg" ||
			path.extension() == ".jpg")
		{
			stbi_write_jpg(
				path_str.c_str(),
				(int)m_Image->dimensions().w, (int)m_Image->dimensions().h,
				Graphics::ImageFormat_GetChannelCount(m_Image->format()),
				buffer->map(),
				90
			);
		} else
		{
			return FileErrorCode::InvalidFormat;
		}

		return FileErrorCode::None;
	}

	void Texture::set_data(WeakRef<const HostImage> _img)
	{
		auto img = _img.lock();

		this->_set_data(img->data(), false);
	}

	void Texture::set_data(const WeakRef<const HostImage> imgs[])
	{
		ArrayList<void*> datas(m_Image->dimensions().layers, m_Image->dimensions().layers);

		for (u32 i = 0; i < m_Image->dimensions().layers; i++)
		{
			datas[i] = imgs[i].lock()->data();
		}

		this->_set_data(datas.ptr(), true);
	}

	void Texture::_set_data(const void* data, bool is_array)
	{
		Graphics::DeviceImageBarrierInfo barrier_info
		{
			.new_img_state = Graphics::DeviceImageState::TransferDst,

			.before = {
				Graphics::BarrierStageBits::Earliest,
				Graphics::BarrierOperationBits::None
			},

			.after = {
				Graphics::BarrierStageBits::Transfer,
				Graphics::BarrierOperationBits::TransferWrite
			}
		};
		m_Image->barrier(barrier_info);

		if (is_array)
		{
			m_Image->set_each_data_2((const void**)data);
		} else
		{
			m_Image->set_all_data(data);
		}

		barrier_info = {
			.new_img_state = Graphics::DeviceImageState::Texture,

			.before = {
				Graphics::BarrierStageBits::Transfer,
				Graphics::BarrierOperationBits::TransferWrite
			},

			.after = {
				Graphics::BarrierStageBits::FragmentShader,
				Graphics::BarrierOperationBits::ShaderRead
			}
		};
		m_Image->barrier(barrier_info);
	}
} // namespace Na::HL
