#include "Pch.hpp"
#include "Natrium/Core/Application.hpp"

#include "Natrium/Core/DeltaTime.hpp"

namespace Na {
	Application::Application(const ApplicationSettings& settings)
	{
		NA_VERIFY(!Application::s_Application, "Failed to create Application: Cannot create more than one Application instance!");
		Application::s_Application = this;

		m_AssetManager = AssetManager(settings.engine_assets_directory, settings.shader_output_directory);

		auto renderer_settings = m_AssetManager.load_asset<RendererSettingsAsset>(settings.renderer_settings_path.data());
		renderer_settings->set_max_anisotropy(Graphics::Device::Get()->limits()->max_anisotropy());

		m_Window = Window(settings.window_width, settings.window_height, settings.window_title);
		m_Renderer = Graphics::Renderer::Make(m_Window, renderer_settings);
	}

	void Application::destroy(void)
	{
		if (!Application::s_Application)
			return;

		Graphics::Device::Get()->wait_all();

		m_LayerManager.detach_all();

		m_Renderer.destroy();
		m_Window.destroy();

		m_LayerManager.destroy();
		m_AssetManager.destroy();

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

			if (m_Window.minimized())
				continue;

			if (!m_Renderer->begin_frame())
				continue;

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

				imgui_layer->end();
			} 
		#endif // NA_DISABLE_IMGUI

			m_Renderer->end_frame();
		}
	}

	void Application::attach_layer(Ref<ImGuiLayer> layer)
	{
		m_LayerManager.attach_layer(layer);
		m_ImGuiLayer = layer;
	}
} // namespace Na
