#if !defined(NA_VULKAN_IMPL_RENDERER_HPP)
#define NA_VULKAN_IMPL_RENDERER_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/Renderer.hpp"

#include "Natrium/Graphics/Buffers.hpp"
#include "Natrium/Graphics/Uniforms.hpp"

#include "Natrium/Graphics/VulkanImpl/vRendererWindow.hpp"

namespace Na::VulkanImpl {
	using ShaderStage = Na::Graphics::ShaderStage;

	struct FrameData {
		bool              valid = false;

		vk::CommandBuffer cmd_buffer;

		vk::Semaphore     image_available_semaphore;
		vk::Semaphore     render_finished_semaphore;
		vk::Fence         in_flight_fence;
	};

	class Renderer : public Graphics::Renderer {
	public:
		Renderer(void) = default;
		Renderer(const Window& window, Ref<const RendererSettingsAsset> settings);

		~Renderer(void) { this->destroy(); }
		void destroy(void) override;

		[[nodiscard]] bool begin_frame(const glm::vec4& color = Colors::k_Black) override;
		void end_frame(void) override;

		void bind_pipeline(View<const Graphics::Pipeline> pipeline) override;

		void set_push_constant(
			u32 size,
			ShaderStage stage,
			u32 offset,
			const void* data,
			View<const Graphics::Pipeline> pipeline
		) const override;

		void draw_vertices(
			View<const Graphics::VertexBuffer> vertex_buffer,
			u32 vertex_count,
			u32 instance_count = 1,
			u32 first_vertex = 0,
			u32 first_instance = 0
		) override;

		void draw_indexed(
			View<const Graphics::VertexBuffer> vertex_buffer,
			View<const Graphics::IndexBuffer> index_buffer,
			u32 instance_count = 1,
			u32 first_index = 0,
			u32 first_instance = 0
		) override;

		void set_descriptor_buffer(
			View<const Graphics::Uniform> buffer,
			const void* data
		) const override;

		[[nodiscard]] inline Ref<const RendererSettingsAsset> settings(void) const override { return m_Window.settings(); }

		[[nodiscard]] inline RendererWindow& window(void) { return m_Window; }
		[[nodiscard]] inline const RendererWindow& window(void) const { return m_Window; }

		[[nodiscard]] inline FrameData& current_frame(void) { return m_Frames[m_FrameIndex]; }
		[[nodiscard]] inline const FrameData& current_frame(void) const { return m_Frames[m_FrameIndex]; }

		[[nodiscard]] inline u32 current_frame_index(void) const { return m_FrameIndex; }

		[[nodiscard]] inline operator bool(void) const { return m_Window; }

		Renderer(const Renderer& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;

		Renderer(Renderer&& other);
		Renderer& operator=(Renderer&& other);
	private:
		void _create_command_objects(void);
		void _create_sync_objects(void);
	private:
		RendererWindow m_Window;

		vk::CommandPool m_GraphicsCmdPool;

		ArrayList<FrameData> m_Frames;
		u32 m_FrameIndex = 0;

		ArrayList<vk::Fence> m_ImageInFlightFences;
		u32 m_ImageIndex = 0;
	};
} // namespace Na

#endif // NA_VULKAN_IMPL_RENDERER_HPP