#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vBarrier.hpp"

#include "Natrium/Graphics/VulkanImpl/vDeviceImage.hpp"

#include "Internal.hpp"

namespace Na::VulkanImpl {
	vk::PipelineStageFlags BarrierStageToVk(BarrierStageBits stage)
	{
		vk::PipelineStageFlags flags;

		if ((stage & BarrierStageBits::Earliest) != BarrierStageBits::None)
			flags |= vk::PipelineStageFlagBits::eTopOfPipe;

		if ((stage & BarrierStageBits::VertexInput) != BarrierStageBits::None)
			flags |= vk::PipelineStageFlagBits::eVertexInput;

		if ((stage & BarrierStageBits::VertexShader) != BarrierStageBits::None)
			flags |= vk::PipelineStageFlagBits::eVertexShader;

		if ((stage & BarrierStageBits::FragmentShader) != BarrierStageBits::None)
			flags |= vk::PipelineStageFlagBits::eFragmentShader;

		if ((stage & BarrierStageBits::ColorAttachmentOutput) != BarrierStageBits::None)
			flags |= vk::PipelineStageFlagBits::eColorAttachmentOutput;

		if ((stage & BarrierStageBits::ComputeShader) != BarrierStageBits::None)
			flags |= vk::PipelineStageFlagBits::eComputeShader;

		if ((stage & BarrierStageBits::Transfer) != BarrierStageBits::None)
			flags |= vk::PipelineStageFlagBits::eTransfer;

		return flags;
	}

	vk::AccessFlags BarrierOperationToVk(BarrierOperationBits op)
	{
		vk::AccessFlags flags;

		if ((op & BarrierOperationBits::ShaderRead) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eShaderRead;

		if ((op & BarrierOperationBits::ShaderWrite) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eShaderWrite;

		if ((op & BarrierOperationBits::TransferRead) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eTransferRead;

		if ((op & BarrierOperationBits::TransferWrite) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eTransferWrite;

		if ((op & BarrierOperationBits::IndexRead) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eIndexRead;

		if ((op & BarrierOperationBits::VertexAttributeRead) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eVertexAttributeRead;

		if ((op & BarrierOperationBits::UniformRead) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eUniformRead;

		if ((op & BarrierOperationBits::ColorAttachmentRead) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eColorAttachmentRead;

		if ((op & BarrierOperationBits::ColorAttachmentWrite) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eColorAttachmentWrite;

		if ((op & BarrierOperationBits::DepthStencilAttachmentRead) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eDepthStencilAttachmentRead;

		if ((op & BarrierOperationBits::DepthStencilAttachmentWrite) != BarrierOperationBits::None)
			flags |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		return flags;
	}

	vk::ImageLayout DeviceImageStateToVk(DeviceImageState state)
	{
		switch (state)
		{
		case DeviceImageState::None:         return vk::ImageLayout::eUndefined;
		case DeviceImageState::Texture:      return vk::ImageLayout::eShaderReadOnlyOptimal;
		case DeviceImageState::StorageImage: return vk::ImageLayout::eGeneral;
		case DeviceImageState::TransferSrc:  return vk::ImageLayout::eTransferSrcOptimal;
		case DeviceImageState::TransferDst:  return vk::ImageLayout::eTransferDstOptimal;
		}
		return vk::ImageLayout::eUndefined;
	}

	void ImageBarrier(
		DeviceImage& img,
		const DeviceImageBarrierInfo& info,
		vk::CommandBuffer cmd_buffer
	)
	{
		vk::ImageMemoryBarrier barrier;

		barrier.oldLayout = img.current_layout();
		barrier.newLayout = DeviceImageStateToVk(info.new_img_state);
		img.set_layout(barrier.newLayout);

		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;

		barrier.image = img.img();
		barrier.subresourceRange = vk::ImageSubresourceRange(
			img.aspect(),
			0, // starting mip level
			1, // mip level count
			0, // starting array layer
			img.layer_count()
		);

		vk::PipelineStageFlags src_stage = BarrierStageToVk(info.before.stage);
		barrier.srcAccessMask = BarrierOperationToVk(info.before.op);

		vk::PipelineStageFlags dst_stage = BarrierStageToVk(info.after.stage);
		barrier.dstAccessMask = BarrierOperationToVk(info.after.op);

		if (cmd_buffer)
		{
			cmd_buffer.pipelineBarrier(
				src_stage,
				dst_stage,
				{}, // dependency flags
				0, nullptr, // memory barriers
				0, nullptr, // buffer memory barriers
				1, &barrier // image memory barriers
			);

			return;
		}

		cmd_buffer = Internal::BeginSingleTimeCommands();

		cmd_buffer.pipelineBarrier(
			src_stage,
			dst_stage,
			{}, // dependency flags
			0, nullptr, // memory barriers
			0, nullptr, // buffer memory barriers
			1, &barrier // image memory barriers
		);

		Internal::EndSingleTimeCommands(cmd_buffer);
	}
} // namespace Na::VulkanImpl
