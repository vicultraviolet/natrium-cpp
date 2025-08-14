#if !defined(NA_VULKAN_IMPL_RENDERER_HPP)
#define NA_VULKAN_IMPL_RENDERER_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/Renderer.hpp"

#include "Natrium/Graphics/Buffer.hpp"
#include "Natrium/Graphics/Uniforms.hpp"
#include "Natrium/Graphics/UniformSet.hpp"

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

		Renderer(const Renderer& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;

		Renderer(Renderer&& other) noexcept;
		Renderer& operator=(Renderer&& other) noexcept;

		[[nodiscard]] bool begin_frame(const glm::vec4& color = Colors::k_Black) override;
		void end_frame(void) override;

		void bind_pipeline(View<const Graphics::Pipeline> pipeline) override;

		void bind_uniform_set(
			View<const Graphics::UniformSet> uniform_set,
			View<const Graphics::Pipeline> pipeline,
			u32 set_index = 0
		);

		void bind_uniform_sets(
			const View<const Graphics::UniformSet>* uniform_sets,
			u64 set_count,
			View<const Graphics::Pipeline> pipeline,
			u32 starting_index = 0
		) override;

		void set_push_constant(
			u32 size,
			ShaderStage stage,
			u32 offset,
			const void* data,
			View<const Graphics::Pipeline> pipeline
		) const override;

		void draw_vertices(
			View<const Graphics::Buffer> vertex_buffer,
			u32 vertex_count,
			u32 instance_count = 1,
			u32 first_vertex = 0,
			u32 first_instance = 0
		) override;

		void draw_indexed(
			View<const Graphics::Buffer> vertex_buffer,
			View<const Graphics::Buffer> index_buffer,
			u32 index_count,
			u32 instance_count = 1,
			u32 first_index = 0,
			u32 first_instance = 0
		) override;

		void set_descriptor_buffer(
			View<const Graphics::Uniform> buffer,
			const void* data
		) const override;

		void dispatch_compute(
			glm::uvec3 workgroup_count
		) override;

		[[nodiscard]] inline const Window& window(void) const override { return m_Window.window(); }
		[[nodiscard]] inline Ref<const RendererSettingsAsset> settings(void) const override { return m_Window.settings(); }

		[[nodiscard]] inline RendererWindow& window_data(void) { return m_Window; }
		[[nodiscard]] inline const RendererWindow& window_data(void) const { return m_Window; }

		[[nodiscard]] inline vk::CommandPool& graphics_cmd_pool(void) { return m_GraphicsCmdPool; }
		[[nodiscard]] inline const vk::CommandPool& graphics_cmd_pool(void) const { return m_GraphicsCmdPool; }

		[[nodiscard]] inline FrameData& current_frame(void) { return m_Frames[m_FrameIndex]; }
		[[nodiscard]] inline const FrameData& current_frame(void) const { return m_Frames[m_FrameIndex]; }

		[[nodiscard]] inline u32 current_frame_index(void) const { return m_FrameIndex; }

		[[nodiscard]] inline vk::DescriptorPool& descriptor_pool(void) { return m_DescriptorPool; }
		[[nodiscard]] inline const vk::DescriptorPool& descriptor_pool(void) const { return m_DescriptorPool; }

		[[nodiscard]] inline operator bool(void) const { return m_Window; }
	private:
		void _create_command_objects(void);
		void _create_sync_objects(void);
		void _create_descriptor_pool(void);
	private:
		RendererWindow m_Window;

		vk::CommandPool m_GraphicsCmdPool = nullptr;

		ArrayList<FrameData> m_Frames;
		u32 m_FrameIndex = 0;

		ArrayList<vk::Fence> m_ImageInFlightFences;
		u32 m_ImageIndex = 0;
		
		vk::DescriptorPool m_DescriptorPool = nullptr;

		std::array<vk::DescriptorSet, 4> m_DescriptorSets{};
		std::array<u32, 32> m_DynamicOffsets{};
	};
} // namespace Na

#endif // NA_VULKAN_IMPL_RENDERER_HPP