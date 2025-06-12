#include "Pch.hpp"
#include "Natrium/Core/Application.hpp"

#include "Natrium/Core/DeltaTime.hpp"

namespace Na {
	Application::Application(
		u32 window_width,
		u32 window_height,
		const std::string_view& window_title,
		const std::filesystem::path& engine_asset_dir,
		const std::filesystem::path& shader_output_dir,
		const std::string& renderer_settings_path
	)
	{
		NA_VERIFY(!Application::s_Application, "Failed to create Application: Cannot create more than one Application instance!");
		Application::s_Application = this;

		m_Context = Context(initialize);

		m_AssetRegistry = AssetRegistry(engine_asset_dir, shader_output_dir);

		auto renderer_settings = m_AssetRegistry.load_renderer_settings(renderer_settings_path);
		renderer_settings->set_max_anisotropy(renderer_settings->AnisotropyLimit());

		m_Window = Window(window_width, window_height, window_title);
		m_Renderer = Renderer(m_Window, renderer_settings);
	}

	void Application::destroy(void)
	{
		Na::VkContext::Get().wait_for_device();

		m_LayerManager.detach_all();

		m_Renderer.destroy();
		m_Window.destroy();

		m_LayerManager.destroy();
		m_AssetRegistry.destroy();

		m_Context.destroy();

		s_Application = nullptr;
	}

	void Application::run(void)
	{
		Na::DeltaTime dt;

		this->running = true;
		while (this->running)
		{
			//g_Logger.printf(Na::Trace, "average fps: {}", (u32)(1.0 / dt));

			for (Na::Event& e : Na::PollEvents())
			{
				if (e.type == Na::EventType::WindowClosed)
				{
					this->running = false;
					return;
				}

				for (Na::LayerHandle<>& layer : m_LayerManager)
				{
					if (!layer->enabled())
						continue;

					layer->on_event(e);
					if (e.handled)
						break;
				}
			}

			dt.calculate();

			for (Na::LayerHandle<>& layer : m_LayerManager)
			{
				if (!layer->updatable())
					continue;

				layer->update(dt);
			}

			if (m_Window.minimized())
				continue;

			if (!m_Renderer.begin_frame())
				continue;

			if (auto imgui_layer = m_ImGuiLayer.lock())
			{
				imgui_layer->begin();

				for (Na::LayerHandle<>& layer : m_LayerManager)
				{
					if (!layer->visible())
						continue;

					layer->draw();
				}

				imgui_layer->end();
			} else
			{
				for (Na::LayerHandle<>& layer : m_LayerManager)
				{
					if (!layer->visible())
						continue;

					layer->draw();
				}
			}

			m_Renderer.end_frame();
		}
	}

	void Application::attach_layer(LayerHandle<ImGuiLayer> layer)
	{
		m_LayerManager.attach_layer(layer);
		m_ImGuiLayer = layer;
	}


	Application::Application(Application&& other)
	: running(std::exchange(other.running, false)),
	m_Context(std::move(other.m_Context)),
	m_AssetRegistry(std::move(other.m_AssetRegistry)),
	m_LayerManager(std::move(other.m_LayerManager)),
	m_Window(std::move(other.m_Window)),
	m_Renderer(std::move(other.m_Renderer))
	{
		Application::s_Application = this;
	}

	Application& Application::operator=(Application&& other)
	{
		this->destroy();

		running = std::exchange(other.running, false);
		m_Context = std::move(other.m_Context);
		m_AssetRegistry = std::move(other.m_AssetRegistry);
		m_LayerManager = std::move(other.m_LayerManager);
		m_Window = std::move(other.m_Window);
		m_Renderer = std::move(other.m_Renderer);

		Application::s_Application = this;
		return *this;
	}
} // namespace Na
