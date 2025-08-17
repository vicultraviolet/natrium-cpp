#include "Pch.hpp"
#include "Natrium/Core/Application.hpp"

#include "Natrium/Core/DeltaTime.hpp"

namespace Na {
	Application::Application(const ApplicationSettings& settings)
	{
		NA_VERIFY(!Application::s_Application, "Failed to create Application: Cannot create more than one Application instance!");
		Application::s_Application = this;

		m_AssetManager = AssetManager(settings.engine_assets_directory, settings.shader_output_directory);

		auto renderer_settings = m_AssetManager.load_asset<RendererSettingsAsset>(settings.renderer_settings_path.data()).value();
		renderer_settings->set_max_anisotropy(Graphics::Device::Get()->limits()->max_anisotropy());

		m_Renderer = Graphics::Renderer::Make(renderer_settings);

		m_Window = Ref<Window>::Make(settings.window_width, settings.window_height, settings.window_title);

		m_RenderTarget = Graphics::SwapchainRenderTarget::Make(m_Window, renderer_settings);
		m_Renderer->bind_render_target(m_RenderTarget);
	}

	void Application::destroy(void)
	{
		if (!Application::s_Application)
			return;

		Graphics::Device::Get()->wait_all();

		m_LayerManager.detach_all();

		Application::s_Application = nullptr;
	}

	void Application::run(void)
	{
		DeltaTime dt;
		while ((this->running = true))
		{
			//g_Logger.printf(Na::Trace, "average fps: {}", (u32)(1.0 / dt));

			for (Event& e : Na::PollEvents())
			{
				if (e.type == Na::EventType::WindowClosed)
				{
					this->running = false;
					return;
				}

				for (Ref<Layer>& layer : m_LayerManager)
				{
					if (!layer->enabled())
						continue;

					layer->on_event(e);
					if (e.handled)
						break;
				}
			}

			dt.calculate();
			m_AverageFPS = (u64)(1.0f / dt);

			for (Ref<Layer>& layer : m_LayerManager)
			{
				if (!layer->updatable())
					continue;

				layer->update(dt);
			}

			if (m_Window->minimized())
				continue;

			if (!m_RenderTarget->acquire_next_image())
				continue;

			m_Renderer->begin_frame();
			m_Renderer->begin_render_pass();

			for (Ref<Layer>& layer : m_LayerManager)
			{
				if (!layer->visible())
					continue;

				layer->draw();
			}

		#if !defined(NA_DISABLE_IMGUI)
			if (auto imgui_layer = m_ImGuiLayer.lock())
			{
				imgui_layer->begin();

				for (Ref<Layer>& layer : m_LayerManager)
				{
					if (!layer->visible())
						continue;

					layer->imgui_draw();
				}

				m_Renderer->draw_imgui();
			} 
		#endif // NA_DISABLE_IMGUI

			m_Renderer->end_render_pass();
			m_Renderer->end_frame();

			m_RenderTarget->present();
		}
	}

	void Application::attach_layer(Ref<ImGuiLayer> layer)
	{
		m_LayerManager.attach_layer(layer);
		m_ImGuiLayer = layer;
	}
} // namespace Na
