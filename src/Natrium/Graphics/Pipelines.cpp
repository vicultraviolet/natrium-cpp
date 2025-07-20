#include "Pch.hpp"
#include "Natrium/Graphics/Pipelines.hpp"

#include "Natrium/Graphics/VulkanImpl/vTrianglePipeline.hpp"

namespace Na::Graphics {
	UniqueRef<TrianglePipeline> TrianglePipeline::Make(
		View<const Renderer> renderer,
		const VertexAttributes& vertex_layout,
		const View<const UniformSetLayout>* uniform_set_layouts,
		u64 uniform_set_layout_count,
		const View<const Shader>* shaders,
		u64 shader_count
	)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::TrianglePipeline>::Make(renderer, vertex_layout, uniform_set_layouts, uniform_set_layout_count, shaders, shader_count);
		}
		return nullptr;
	}

	UniqueRef<TrianglePipeline> TrianglePipeline::Make(
		View<const Renderer> renderer,
		const VertexAttributes& vertex_layout,
		const UniformLayout& uniform_layout,
		const Shaders& shaders
	)
	{
		return TrianglePipeline::Make(
			renderer,
			vertex_layout,
			uniform_layout.begin(), uniform_layout.size(),
			shaders.begin(), shaders.size()
		);
	}
} // namespace Na::Graphics
