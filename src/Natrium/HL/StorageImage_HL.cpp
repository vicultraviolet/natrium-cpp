#include "Pch.hpp"
#include "Natrium/HL/StorageImage_HL.hpp"

namespace Na::HL {
	StorageImage::StorageImage(u32 width, u32 height)
	{
		Graphics::DeviceImageCreateInfo img_info
		{
			.width = width,
			.height = height,

			.layer_count = 1,

			.format = Graphics::ImageFormat::Rgba8,
			.type = Graphics::DeviceImageTypeFlags::Storage
		};
		m_Image = Graphics::DeviceImage::Make(img_info);

		m_Image->set_stage(Graphics::DeviceImageStage::StorageImage);
	}
} // namespace Na::HL
