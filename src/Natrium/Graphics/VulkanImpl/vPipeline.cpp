#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vPipeline.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

namespace Na::VulkanImpl {
	vk::PipelineBindPoint PipelineTypeToVk(PipelineType type)
	{
		switch (type)
		{
		case PipelineType::Triangle: return vk::PipelineBindPoint::eGraphics;
		case PipelineType::Compute:  return vk::PipelineBindPoint::eCompute;
		}
		return vk::PipelineBindPoint(-1);
	}

	void Pipeline::destroy(void)
	{
		const auto& logical_device = Device::Get()->logical_device();

		if (this->pipeline)
		{
			logical_device.destroyPipeline(this->pipeline);
			this->pipeline = nullptr;
		}

		if (this->layout)
		{
			logical_device.destroyPipelineLayout(this->layout);
			this->layout = nullptr;
		}
	}
} // namespace Na::VulkanImpl
