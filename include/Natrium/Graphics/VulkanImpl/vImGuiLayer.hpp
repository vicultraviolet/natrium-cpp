#if !defined(NA_VULKAN_IMPL_IMGUI_LAYER_HPP)
#define NA_VULKAN_IMPL_IMGUI_LAYER_HPP

#include "Natrium/Layers/ImGuiLayer.hpp"

namespace Na::VulkanImpl {
#if !defined(NA_DISABLE_IMGUI)
	class ImGuiLayer : public Na::ImGuiLayer {
	public:
		ImGuiLayer(
			WeakRef<Graphics::SwapchainRenderTarget> render_target,
			i64 priority = 0,
			bool demo_window_shown = false
		);
		~ImGuiLayer(void);

		void begin(void) override;
	private:
		vk::DescriptorPool m_DescriptorPool = nullptr;
	};
#endif // NA_DISABLE_IMGUI
} // namespace Na

#endif // NA_VULKAN_IMPL_IMGUI_LAYER_HPP