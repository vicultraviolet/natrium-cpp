#if !defined(NA_GRAPHICS_RENDER_TARGETS_HPP)
#define NA_GRAPHICS_RENDER_TARGETS_HPP

#include "Natrium/Core/Window.hpp"
#include "Natrium/Graphics/RendererSettings.hpp"

namespace Na::Graphics {
	enum class RenderTargetType : u8 {
		None = 0,
		Swapchain
	};

	struct Viewport {
		union {
			struct {
				float x;
				float y;
			};
			glm::vec2 pos;
		};

		union {
			struct {
				float width;
				float height;
			};
			glm::vec2 size;
		};

		float min_depth = 0.0f;
		float max_depth = 1.0f;

		Viewport(void) = default;
	};

	struct Scissor {
		union {
			struct {
				i32 x;
				i32 y;
			};
			glm::ivec2 pos;
		};
		union {
			struct {
				u32 width;
				u32 height;
			};
			glm::uvec2 size;
		};
	};

	class RenderTarget {
	public:
		virtual ~RenderTarget(void) = default;

		[[nodiscard]] virtual u32 width(void) const = 0;
		[[nodiscard]] virtual u32 height(void) const = 0;

		virtual void set_viewport(const Viewport& viewport) = 0;
		virtual void set_scissor(const Scissor& scissor) = 0;

		[[nodiscard]] virtual const Viewport& viewport(void) const = 0;
		[[nodiscard]] virtual const Scissor& scissor(void) const = 0;

		[[nodiscard]] virtual Ref<const RendererSettings> renderer_settings(void) const = 0;

		[[nodiscard]] virtual RenderTargetType type(void) const = 0;
	};

	class SwapchainRenderTarget : public RenderTarget {
	public:
		[[nodiscard]] static Ref<SwapchainRenderTarget> Make(
			WeakRef<const Window> window,
			Ref<const RendererSettings> renderer_settings
		);
		virtual ~SwapchainRenderTarget(void) = default;

		[[nodiscard]] virtual bool acquire_next_image(void) = 0;
		virtual void present(void) = 0;

		virtual void recreate_swapchain(void) = 0;

		[[nodiscard]] virtual u32 image_count(void) const = 0;
		[[nodiscard]] virtual u32 current_image_index(void) const = 0;

		[[nodiscard]] virtual u32 current_frame_index(void) const = 0;

		[[nodiscard]] virtual WeakRef<const Window> window(void) const = 0;

		[[nodiscard]] inline RenderTargetType type(void) const override { return RenderTargetType::Swapchain; }
		[[nodiscard]] static inline RenderTargetType GetType(void) { return RenderTargetType::Swapchain; }
	};
} // namespace Na::Graphics

#endif // NA_GRAPHICS_RENDER_TARGETS_HPP