#if !defined(NA_VULKAN_IMPL_PIPELINE_HPP)
#define NA_VULKAN_IMPL_PIPELINE_HPP

#include "Natrium/Graphics/Pipelines.hpp"

namespace Na::VulkanImpl {
	using PipelineType = Graphics::PipelineType;

	vk::PipelineBindPoint PipelineTypeToVk(PipelineType type);

	struct Pipeline {
		vk::Pipeline pipeline = nullptr;
		vk::PipelineLayout layout = nullptr;

		void destroy(void);

		[[nodiscard]] inline operator bool(void) const { return this->pipeline && this->layout; }
	};

	constexpr u64 k_PipelineOffset = 8;
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_PIPELINE_HPP