#include "Pch.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

#include "Natrium/Graphics/VulkanImpl/vPipeline.hpp"

namespace Na::Graphics {
	UniqueRef<Pipeline> Pipeline::Make(
		View<const Renderer> renderer,
		const VertexAttributes& vertex_shader_layout,
		const std::initializer_list<View<const Shader>>& shaders
	)
	{
		switch (Device::Get().backend())
		{
			case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::Pipeline>::Make(renderer, vertex_shader_layout, shaders);
		}
		return nullptr;
	}
} // namespace Na::Graphics
