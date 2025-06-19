#if !defined(NA_RENDERER_CORE_HPP)
#define NA_RENDERER_CORE_HPP

#include "Natrium/Core/Window.hpp"
#include "Natrium/Graphics/DeviceImage.hpp"
#include "Natrium/Graphics/Colors.hpp"

#include "Natrium/Assets/RendererSettingsAsset.hpp"

namespace Na {
	class RendererCore {
	public:
		RendererCore(void) = default;
		~RendererCore(void) { this->destroy(); }

		RendererCore(Window& window, AssetHandle<RendererSettings> settings);

		void destroy(void);

		RendererCore(const RendererCore& other) = delete;
		RendererCore& operator=(const RendererCore& other) = delete;

		RendererCore(RendererCore&& other);
		RendererCore& operator=(RendererCore&& other);

		[[nodiscard]] inline AssetHandle<RendererSettings> settings(void) const { return m_Settings; }

		[[nodiscard]] inline u32 width(void) const { return m_Width; }
		[[nodiscard]] inline u32 height(void) const { return m_Height; }

		inline void set_viewport(const glm::vec4& viewport);
		[[nodiscard]] inline glm::vec4 viewport(void) const { return glm::vec4(m_Viewport.x, m_Viewport.y, m_Viewport.width, m_Viewport.height); }

		[[nodiscard]] inline Window& window(void) { return *m_Window; }
		[[nodiscard]] inline const Window& window(void) const { return *m_Window; }

		[[nodiscard]] inline vk::SurfaceKHR surface(void) const { return m_Surface; }

		[[nodiscard]] inline vk::SwapchainKHR swapchain(void) const { return m_Swapchain; }
		[[nodiscard]] inline vk::SurfaceFormatKHR swapchain_format(void) const { return m_SwapchainFormat; }

		[[nodiscard]] inline const Na::ArrayList<vk::Image>& images(void) const { return m_Images; }
		[[nodiscard]] inline const Na::ArrayList<vk::ImageView>& image_views(void) const { return m_ImageViews; }

		[[nodiscard]] inline const DeviceImage& color_image(void) const { return m_ColorImage; }
		[[nodiscard]] inline vk::ImageView color_image_view(void) const { return m_ColorImageView; }

		[[nodiscard]] inline const DeviceImage& depth_image(void) const { return m_DepthImage; }
		[[nodiscard]] inline vk::ImageView depth_image_view(void) const { return m_DepthImageView; }

		[[nodiscard]] inline const ArrayList<vk::Framebuffer>& framebuffers(void) const { return m_Framebuffers; }

		[[nodiscard]] inline vk::RenderPass render_pass(void) const { return m_RenderPass; }

		[[nodiscard]] inline operator bool(void) const { return m_Window; }
	private:
		void _create_window_surface(void);
		void _create_swapchain(void);
		void _create_image_views(void);
		void _create_color_buffer(void);
		void _create_depth_buffer(void);
		void _create_render_pass(void);
		void _create_framebuffers(void);

		void _recreate_swapchain(void);
	private:
		friend class Renderer;

		Window* m_Window = nullptr;
		vk::SurfaceKHR m_Surface;
		u32 m_Width, m_Height;

		vk::Viewport m_Viewport;
		vk::Rect2D m_Scissor;

		vk::SwapchainKHR m_Swapchain;
		vk::SurfaceFormatKHR m_SwapchainFormat;

		Na::ArrayList<vk::Image> m_Images;
		Na::ArrayList<vk::ImageView> m_ImageViews;

		DeviceImage m_ColorImage;
		vk::ImageView m_ColorImageView;

		DeviceImage m_DepthImage;
		vk::ImageView m_DepthImageView;

		vk::RenderPass m_RenderPass;
		ArrayList<vk::Framebuffer> m_Framebuffers;

		AssetHandle<RendererSettings> m_Settings;
	};
} // namespace Na

#endif // NA_RENDERER_CORE_HPP