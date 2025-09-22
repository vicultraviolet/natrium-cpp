#if !defined(NA_VULKAN_IMPL_SWAPCHAIN_RENDER_TARGET_HPP)
#define NA_VULKAN_IMPL_SWAPCHAIN_RENDER_TARGET_HPP

#include "Natrium/Graphics/RenderTargets.hpp"
#include "Natrium/Core/Window.hpp"

#include "Natrium/Graphics/VulkanImpl/vDeviceImage.hpp"

namespace Na::VulkanImpl {
	using RenderTargetType = Graphics::RenderTargetType;

	using Viewport = Graphics::Viewport;
	using Scissor = Graphics::Scissor;

	struct FrameData {
		vk::Semaphore image_available_semaphore = nullptr;
		vk::Semaphore render_finished_semaphore = nullptr;
		vk::Fence     in_flight_fence = nullptr;
	};

	struct RenderTargetSubmitInfo {
		vk::Semaphore wait_semaphore = nullptr;
		vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::Semaphore signal_semaphore = nullptr;

		vk::Fence fence = nullptr;
	};

	class SwapchainRenderTarget : public Graphics::SwapchainRenderTarget {
	public:
		SwapchainRenderTarget(
			WeakRef<const Window> window,
			Ref<const RendererSettings> renderer_settings
		);
		~SwapchainRenderTarget(void) { this->destroy(); }

		void destroy(void);

		[[nodiscard]] bool acquire_next_image(void) override;
		void present(void) override;

		void recreate_swapchain(void);

		[[nodiscard]] inline u32 width(void) const { return m_Width; }
		[[nodiscard]] inline u32 height(void) const { return m_Height; }

		void set_viewport(const Viewport& viewport) override;
		void set_scissor(const Scissor& scissor) override;

		[[nodiscard]] inline const Viewport& viewport(void) const override { return m_Viewport; }
		[[nodiscard]] inline const Scissor& scissor(void) const override { return m_Scissor; }

		[[nodiscard]] inline u32 image_count(void) const { return (u32)m_Images.size(); }
		[[nodiscard]] inline u32 current_image_index(void) const { return m_ImageIndex; }

		[[nodiscard]] inline u32 current_frame_index(void) const { return m_FrameIndex; }

		[[nodiscard]] inline WeakRef<const Window> window(void) const { return m_Window; }

		[[nodiscard]] inline Ref<const RendererSettings> renderer_settings(void) const { return m_RendererSettings; }

		[[nodiscard]] RenderTargetSubmitInfo submit_info(void) const;

		void begin_render_pass(vk::CommandBuffer cmd_buffer, const glm::vec4& clear_color) const;

		[[nodiscard]] inline vk::SurfaceKHR& surface(void) { return m_Surface; }
		[[nodiscard]] inline const vk::SurfaceKHR& surface(void) const { return m_Surface; }

		[[nodiscard]] inline vk::SwapchainKHR& swapchain(void) { return m_Swapchain; }
		[[nodiscard]] inline const vk::SwapchainKHR& swapchain(void) const { return m_Swapchain; }

		[[nodiscard]] inline vk::SurfaceFormatKHR swapchain_format(void) const { return m_SwapchainFormat; }

		[[nodiscard]] inline const ArrayList<vk::Image>& images(void) const { return m_Images; }
		[[nodiscard]] inline const ArrayList<vk::ImageView>& image_views(void) const { return m_ImageViews; }

		[[nodiscard]] inline DeviceImage& color_image(void) { return m_ColorImage; }
		[[nodiscard]] inline const DeviceImage& color_image(void) const { return m_ColorImage; }

		[[nodiscard]] inline DeviceImage& depth_image(void) { return m_DepthImage; }
		[[nodiscard]] inline const DeviceImage& depth_image(void) const { return m_DepthImage; }

		[[nodiscard]] inline vk::RenderPass& render_pass(void) { return m_RenderPass; }
		[[nodiscard]] inline const vk::RenderPass& render_pass(void) const { return m_RenderPass; }

		[[nodiscard]] inline const vk::Framebuffer& current_framebuffer(void) const { return m_Framebuffers[m_ImageIndex]; }
		[[nodiscard]] inline vk::Framebuffer& current_framebuffer(void) { return m_Framebuffers[m_ImageIndex]; }

		[[nodiscard]] inline const ArrayList<vk::Framebuffer>& framebuffers(void) const { return m_Framebuffers; }

		[[nodiscard]] inline RenderTargetType type(void) const override { return RenderTargetType::Swapchain; }
		[[nodiscard]] static inline RenderTargetType GetType(void) { return RenderTargetType::Swapchain; }
	private:
		void _create_window_surface(void);
		void _create_swapchain(void);
		void _create_image_views(void);
		void _create_color_buffer(void);
		void _create_depth_buffer(void);
		void _create_render_pass(void);
		void _create_framebuffers(void);
		void _create_sync_objects(void);
	private:
		WeakRef<const Window> m_Window;

		vk::SurfaceKHR m_Surface;
		u32 m_Width, m_Height;

		union {
			vk::Viewport m_ViewportVk;
			Viewport m_Viewport;
		};

		union {
			vk::Rect2D m_ScissorVk;
			Scissor m_Scissor;
		};

		vk::SwapchainKHR m_Swapchain;
		vk::SurfaceFormatKHR m_SwapchainFormat;

		Na::ArrayList<vk::Image> m_Images;
		Na::ArrayList<vk::ImageView> m_ImageViews;

		DeviceImage m_ColorImage;
		DeviceImage m_DepthImage;

		vk::RenderPass m_RenderPass;
		ArrayList<vk::Framebuffer> m_Framebuffers;

		ArrayList<FrameData> m_Frames;
		u32 m_FrameIndex = 0;

		ArrayList<vk::Fence> m_ImageInFlightFences;
		u32 m_ImageIndex = 0;

		Ref<const RendererSettings> m_RendererSettings;
	};
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_SWAPCHAIN_RENDER_TARGET_HPP