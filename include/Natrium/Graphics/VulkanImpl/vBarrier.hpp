#if !defined(NA_VULKAN_IMPL_BARRIER_HPP)
#define NA_VULKAN_IMPL_BARRIER_HPP

#include "Natrium/Graphics/Barrier.hpp"

namespace Na::VulkanImpl {
	class DeviceImage;

	using BarrierStageBits = Graphics::BarrierStageBits;
	using BarrierOperationBits = Graphics::BarrierOperationBits;

	using BarrierOperationInfo = Graphics::BarrierOperationInfo;

	using DeviceImageState = Graphics::DeviceImageState;

	using DeviceImageBarrierInfo = Graphics::DeviceImageBarrierInfo;

	vk::PipelineStageFlags BarrierStageToVk(BarrierStageBits stage);
	vk::AccessFlags BarrierOperationToVk(BarrierOperationBits op);

	vk::ImageLayout DeviceImageStateToVk(DeviceImageState state);

	void ImageBarrier(
		DeviceImage& img,
		const DeviceImageBarrierInfo& info,
		vk::CommandBuffer cmd_buffer = nullptr
	);
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_BARRIER_HPP