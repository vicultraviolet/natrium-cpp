#include "Pch.hpp"
#include "Natrium/Graphics/Pipelines.hpp"

#include "Natrium/Graphics/VulkanImpl/vTrianglePipeline.hpp"
#include "Natrium/Graphics/VulkanImpl/vComputePipeline.hpp"

namespace Na::Graphics {
	UniqueRef<TrianglePipeline> TrianglePipeline::Make(
		WeakRef<const Graphics::RenderTarget> render_target,
		const VertexAttributes& vertex_layout,
		const View<const UniformSetLayout>* uniform_set_layouts,
		u64 uniform_set_layout_count,
		const View<const Shader>* shaders,
		u64 shader_count
	)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::TrianglePipeline>::Make(render_target, vertex_layout, uniform_set_layouts, uniform_set_layout_count, shaders, shader_count);
		}
		return nullptr;
	}

	UniqueRef<TrianglePipeline> TrianglePipeline::Make(
		WeakRef<const Graphics::RenderTarget> render_target,
		const VertexAttributes& vertex_layout,
		const UniformLayout& uniform_layout,
		const Shaders& shaders
	)
	{
		return TrianglePipeline::Make(
			render_target,
			vertex_layout,
			uniform_layout.begin(), uniform_layout.size(),
			shaders.begin(), shaders.size()
		);
	}

	UniqueRef<ComputePipeline> ComputePipeline::Make(
		View<const Shader> shader,
		const View<const UniformSetLayout>* uniform_set_layouts,
		u64 uniform_set_layout_count
	)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::ComputePipeline>::Make(shader, uniform_set_layouts, uniform_set_layout_count);
		}
		return nullptr;
	}

	UniqueRef<ComputePipeline> ComputePipeline::Make(
		View<const Shader> shader,
		const UniformLayout& uniform_layout
	)
	{
		return ComputePipeline::Make(shader, uniform_layout.begin(), uniform_layout.size());
	}
} // namespace Na::Graphics
