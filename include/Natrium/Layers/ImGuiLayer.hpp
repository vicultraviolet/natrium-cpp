#if !defined(NA_IMGUI_LAYER_HPP)
#define NA_IMGUI_LAYER_HPP

#include "Natrium/Layers/Layer.hpp"
#include "Natrium/Graphics/Renderer.hpp"

namespace Na {
#if !defined(NA_DISABLE_IMGUI)
	class ImGuiLayer : public Layer {
	public:
		[[nodiscard]] static UniqueRef<ImGuiLayer> Make(
			View<const Graphics::Renderer> renderer,
			i64 priority = 0,
			bool demo_window_shown = false
		);

		ImGuiLayer(View<const Graphics::Renderer> renderer, i64 priority = 0, bool demo_window_shown = false);
		virtual ~ImGuiLayer(void);

		virtual void begin(void) = 0;
		virtual void end(void) const = 0;

		void on_event(Event& e) override;
		void imgui_draw(void) override;

		[[nodiscard]] inline bool demo_window_shown(void) const { return m_DemoWindowShown; }
		inline void set_demo_window_shown(bool shown) { m_DemoWindowShown = shown; }

		[[nodiscard]] inline View<const Graphics::Renderer> renderer(void) const { return m_Renderer; }
	private:
		View<const Graphics::Renderer> m_Renderer = nullptr;
		bool m_DemoWindowShown = false;
	};
#endif // NA_DISABLE_IMGUI
} // namespace Na

#endif // NA_IMGUI_LAYER_HPP