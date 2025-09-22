#include "Pch.hpp"
#include "Natrium/Graphics/Renderer.hpp"

#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"

namespace Na::Graphics {
	UniqueRef<Renderer> Renderer::Make(Ref<const RendererSettings> settings)
	{
		switch (Device::Get()->backend())
		{
		case DeviceBackend::Vulkan: return MakeUnique<VulkanImpl::Renderer>(settings);
		}
		return nullptr;
	}

	void Renderer::bind_uniform_sets(
		const std::initializer_list<View<const UniformSet>>& uniform_sets,
		View<const Pipeline> pipeline,
		u32 starting_index
	)
	{
		return this->bind_uniform_sets(uniform_sets.begin(), uniform_sets.size(), pipeline, starting_index);
	}
} // namespace Na::Graphics
