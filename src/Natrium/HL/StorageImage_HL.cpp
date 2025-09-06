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

		Graphics::DeviceImageBarrierInfo barrier_info
		{
			.new_img_state = Graphics::DeviceImageState::StorageImage,

			.before = {
				Graphics::BarrierStageBits::Earliest,
				Graphics::BarrierOperationBits::None
			},

			.after = {
				Graphics::BarrierStageBits::ComputeShader,
				Graphics::BarrierOperationBits::ShaderRead | Graphics::BarrierOperationBits::ShaderWrite
			}
		};
		m_Image->barrier(barrier_info);
	}
} // namespace Na::HL
