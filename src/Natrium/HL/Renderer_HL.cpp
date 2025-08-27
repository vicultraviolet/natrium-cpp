#include "Pch.hpp"
#include "Natrium/HL/Renderer_HL.hpp"

namespace Na::HL {
	Renderer::Renderer(Ref<const RendererSettingsAsset> settings)
	: m_Renderer(Graphics::Renderer::Make(settings))
	{

	}

	void Renderer::begin_frame(Ref<Display> display)
	{
		if (display->window().lock()->minimized())
		{
			m_ShouldRender = false;
			return;
		}

		bool valid = display->acquire_next_image();
		if (m_ShouldRender = valid)
		{
			m_Renderer->bind_render_target(display);
			m_Renderer->begin_frame();
		}
	}

	void Renderer::end_frame(void)
	{
		if (!m_ShouldRender)
			return;

		if (m_RenderPassStarted)
			this->end_render_pass();

		m_Renderer->end_frame();
	}

	void Renderer::begin_render_pass(const glm::vec4& clear_color)
	{
		m_Renderer->begin_render_pass(clear_color);
		m_RenderPassStarted = true;
	}

	void Renderer::end_render_pass(void)
	{
		m_Renderer->end_render_pass();
		m_RenderPassStarted = false;
	}

	void Renderer::clear(Ref<Display> display, const glm::vec4& clear_color)
	{
		this->begin_frame(display);

		if (!m_ShouldRender)
			return;

		this->begin_render_pass(clear_color);
	}

	void Renderer::present(void)
	{
		if (!m_ShouldRender)
			return;

		this->end_frame();
		if (auto render_target = m_Renderer->current_render_target().lock())
		{
			if (auto display = dynamic_ref_cast<Display>(render_target))
			{
				display->present();
			}
		}
	}
} // namespace Na::HL
