#if !defined(NA_HL_RENDERER_HPP)
#define NA_HL_RENDERER_HPP

#include "Natrium/Graphics/Renderer.hpp"

namespace Na::HL {
	using Display = Graphics::SwapchainRenderTarget;

	class Renderer {
	public:
		Renderer(Ref<const RendererSettings> settings);
		~Renderer(void) = default;
		
		void begin_frame(Ref<Display> display);
		void end_frame(void);

		void begin_render_pass(const glm::vec4& clear_color = Colors::k_Black);
		void end_render_pass(void);

		void clear(Ref<Display> display, const glm::vec4& clear_color = Colors::k_Black);
		void present(void);

		[[nodiscard]] inline Ref<Graphics::Renderer> renderer(void) { return m_Renderer; }
		[[nodiscard]] inline Ref<const Graphics::Renderer> renderer(void) const { return m_Renderer; }

		[[nodiscard]] inline bool should_render(void) const { return m_ShouldRender; }
		[[nodiscard]] inline bool render_pass_started(void) const { return m_RenderPassStarted; }

		[[nodiscard]] inline u32 max_frames_in_flight(void) const { return m_Renderer->settings()->max_frames_in_flight; }
		[[nodiscard]] inline u32 current_frame_index(void) const { return m_Renderer->current_frame_index(); }
	private:
		Ref<Graphics::Renderer> m_Renderer;
		bool m_ShouldRender = true;
		bool m_RenderPassStarted = false;
	};
} // namespace Na::HL

#endif // NA_HL_RENDERER_HPP