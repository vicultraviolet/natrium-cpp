#if !defined(NA_RENDERER_WINDOW_HPP)
#define NA_RENDERER_WINDOW_HPP

#include "Natrium/Core/Window.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceImage.hpp"
#include "Natrium/Graphics/Colors.hpp"

#include "Natrium/Assets/RendererSettingsAsset.hpp"

namespace Na::VulkanImpl {
	class RendererWindow {
	public:
		RendererWindow(void) = default;
		RendererWindow(const Window& window, Ref<const RendererSettingsAsset> settings);

		~RendererWindow(void) { this->destroy(); }

		void destroy(void);

		RendererWindow(const RendererWindow& other) = delete;
		RendererWindow& operator=(const RendererWindow& other) = delete;

		RendererWindow(RendererWindow&& other);
		RendererWindow& operator=(RendererWindow&& other);

		void recreate_swapchain(void);

		[[nodiscard]] inline Ref<const RendererSettingsAsset> settings(void) const { return m_Settings; }

		[[nodiscard]] inline u32 width(void) const { return m_Width; }
		[[nodiscard]] inline u32 height(void) const { return m_Height; }

		[[nodiscard]] inline u32 image_count(void) const { return (u32)m_Images.size(); }

		void set_viewport(const vk::Viewport& viewport);

		[[nodiscard]] inline vk::Viewport& viewport(void) { return m_Viewport; }
		[[nodiscard]] inline const vk::Viewport& viewport(void) const { return m_Viewport; }

		[[nodiscard]] inline vk::Rect2D& scissor(void) { return m_Scissor; }
		[[nodiscard]] inline const vk::Rect2D& scissor(void) const { return m_Scissor; }

		[[nodiscard]] inline const Window& window(void) const { return *m_Window; }

		[[nodiscard]] inline vk::SurfaceKHR& surface(void) { return m_Surface; }
		[[nodiscard]] inline vk::SurfaceKHR surface(void) const { return m_Surface; }

		[[nodiscard]] inline vk::SwapchainKHR& swapchain(void) { return m_Swapchain; }
		[[nodiscard]] inline vk::SwapchainKHR swapchain(void) const { return m_Swapchain; }

		[[nodiscard]] inline vk::SurfaceFormatKHR swapchain_format(void) const { return m_SwapchainFormat; }

		[[nodiscard]] inline const ArrayList<vk::Image>& images(void) const { return m_Images; }
		[[nodiscard]] inline const ArrayList<vk::ImageView>& image_views(void) const { return m_ImageViews; }

		[[nodiscard]] inline DeviceImage& color_image(void) { return m_ColorImage; }
		[[nodiscard]] inline const DeviceImage& color_image(void) const { return m_ColorImage; }

		[[nodiscard]] inline vk::ImageView& color_image_view(void) { return m_ColorImageView; }
		[[nodiscard]] inline vk::ImageView color_image_view(void) const { return m_ColorImageView; }

		[[nodiscard]] inline DeviceImage& depth_image(void) { return m_DepthImage; }
		[[nodiscard]] inline const DeviceImage& depth_image(void) const { return m_DepthImage; }

		[[nodiscard]] inline vk::ImageView& depth_image_view(void) { return m_DepthImageView; }
		[[nodiscard]] inline vk::ImageView depth_image_view(void) const { return m_DepthImageView; }

		[[nodiscard]] inline vk::RenderPass& render_pass(void) { return m_RenderPass; }
		[[nodiscard]] inline vk::RenderPass render_pass(void) const { return m_RenderPass; }

		[[nodiscard]] inline const ArrayList<vk::Framebuffer>& framebuffers(void) const { return m_Framebuffers; }

		[[nodiscard]] inline operator bool(void) const { return m_Window; }
	private:
		void _create_window_surface(void);
		void _create_swapchain(void);
		void _create_image_views(void);
		void _create_color_buffer(void);
		void _create_depth_buffer(void);
		void _create_render_pass(void);
		void _create_framebuffers(void);
	private:
		View<const Window> m_Window;

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

		Ref<const RendererSettingsAsset> m_Settings;
	};
} // namespace Na

#endif // NA_RENDERER_WINDOW_HPP