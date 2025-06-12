#if !defined(NA_APPLICATION_HPP)
#define NA_APPLICATION_HPP

#include "Natrium/Core/Context.hpp"
#include "Natrium/Core/Window.hpp"
#include "Natrium/Layers/LayerManager.hpp"
#include "Natrium/Assets/AssetRegistry.hpp"
#include "Natrium/Graphics/Renderer/Renderer.hpp"
#include "Natrium/Layers/ImGuiLayer.hpp"

namespace Na {
	class Application {
	public:
		bool running = false;

		Application(void) = default;
		Application(
			u32 window_width,
			u32 window_height,
			const std::string_view& window_title,
			const std::filesystem::path& engine_asset_dir,
			const std::filesystem::path& shader_output_dir,
			const std::string& renderer_settings_path
		);
		~Application(void) { this->destroy(); }

		void destroy(void);

		Application(const Application& other) = delete;
		Application& operator=(const Application& other) = delete;

		Application(Application&& other);
		Application& operator=(Application&& other);

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

		[[nodiscard]] static inline Application& Get(void) { return *s_Application; }
		[[nodiscard]] static inline bool Exists(void) { return s_Application; }

		[[nodiscard]] inline AssetRegistry& asset_registry(void) { return m_AssetRegistry; }
		[[nodiscard]] inline const AssetRegistry& asset_registry(void) const { return m_AssetRegistry; }

		[[nodiscard]] inline LayerManager& layer_manager(void) { return m_LayerManager; }
		[[nodiscard]] inline const LayerManager& layer_manager(void) const { return m_LayerManager; }

		[[nodiscard]] inline Window& window(void) { return m_Window; }
		[[nodiscard]] inline const Window& window(void) const { return m_Window; }

		[[nodiscard]] inline Renderer& renderer(void) { return m_Renderer; }
		[[nodiscard]] inline const Renderer& renderer(void) const { return m_Renderer; }

		[[nodiscard]] inline WeakLayerHandle<ImGuiLayer> imgui_layer(void) const { return m_ImGuiLayer; }
	private:
		Context m_Context;

		AssetRegistry m_AssetRegistry;
		LayerManager m_LayerManager;

		Window m_Window;
		Renderer m_Renderer;

		WeakLayerHandle<ImGuiLayer> m_ImGuiLayer;

		static inline Application* s_Application = nullptr;
	};
	using App = Application;
} // namespace Na

#endif // NA_APPLICATION_HPP