#if !defined(NA_APPLICATION_HPP)
#define NA_APPLICATION_HPP

#include "Natrium/Core/Context.hpp"
#include "Natrium/Core/Window.hpp"
#include "Natrium/Layers/LayerManager.hpp"
#include "Natrium/Assets/AssetManager.hpp"
#include "Natrium/Graphics/Renderer.hpp"
#include "Natrium/Layers/ImGuiLayer.hpp"

namespace Na {
	struct ApplicationSettings {
		u32 window_width = 1280;
		u32 window_height = 720;
		std::string_view window_title = "Natrium Application";
		std::filesystem::path engine_assets_directory = "assets/engine";
		std::filesystem::path shader_output_directory = "bin/shaders";
		std::string_view renderer_settings_path = "renderer_settings.json";
	};

	class Application {
	public:
		bool running = false;

		Application(void) = default;
		explicit Application(const ApplicationSettings& settings);

		~Application(void) { this->destroy(); }
		void destroy(void);

		Application(const Application& other) = delete;
		Application& operator=(const Application& other) = delete;

		void run(void);

		inline void attach_layer(LayerHandle<> layer) { m_LayerManager.attach_layer(layer); }
		inline void detach_layer(LayerHandle<> layer) { m_LayerManager.detach_layer(layer); }

		void attach_layer(LayerHandle<ImGuiLayer> layer);

		template<typename T, typename... t_Args>
		LayerHandle<T> create_layer(t_Args&&... __args)
		{
			LayerHandle<T> layer = CreateLayer<T>(std::forward<t_Args>(__args)...);
			this->attach_layer(layer);
			return layer;
		}

		template<typename... t_Args>
		LayerHandle<ImGuiLayer> create_imgui_layer(t_Args&&... __args)
		{
			LayerHandle<ImGuiLayer> layer = ImGuiLayer::Make(std::forward<t_Args>(__args)...);
			this->attach_layer(layer);
			return layer;
		}

		[[nodiscard]] static inline Application& Get(void) { return *Application::s_Application; }
		[[nodiscard]] static inline bool Exists(void) { return Application::s_Application; }

		[[nodiscard]] inline AssetManager& asset_manager(void) { return m_AssetManager; }
		[[nodiscard]] inline const AssetManager& asset_manager(void) const { return m_AssetManager; }

		[[nodiscard]] inline LayerManager& layer_manager(void) { return m_LayerManager; }
		[[nodiscard]] inline const LayerManager& layer_manager(void) const { return m_LayerManager; }

		[[nodiscard]] inline Window& window(void) { return m_Window; }
		[[nodiscard]] inline const Window& window(void) const { return m_Window; }

		[[nodiscard]] inline View<Graphics::Renderer> renderer(void) { return m_Renderer; }
		[[nodiscard]] inline View<const Graphics::Renderer> renderer(void) const { return m_Renderer; }

		[[nodiscard]] inline WeakLayerHandle<ImGuiLayer> imgui_layer(void) const { return m_ImGuiLayer; }

		[[nodiscard]] inline u64 average_fps(void) const { return m_AverageFPS; }
	private:
		AssetManager m_AssetManager;
		LayerManager m_LayerManager;

		Window m_Window;
		UniqueRef<Graphics::Renderer> m_Renderer;

		WeakLayerHandle<ImGuiLayer> m_ImGuiLayer;

		u64 m_AverageFPS = 0;

		static inline Application* s_Application = nullptr;
	};
	using App = Application;
} // namespace Na

#endif // NA_APPLICATION_HPP