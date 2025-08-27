#if !defined(NA_VULKAN_IMPL_RENDERER_HPP)
#define NA_VULKAN_IMPL_RENDERER_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/Renderer.hpp"

#include "Natrium/Graphics/Buffer.hpp"
#include "Natrium/Graphics/UniformSet.hpp"

#include "Natrium/Graphics/VulkanImpl/vSwapchainRenderTarget.hpp"

namespace Na::VulkanImpl {
	using ShaderStage = Na::Graphics::ShaderStage;

	class Renderer : public Graphics::Renderer {
	public:
		Renderer(Ref<const RendererSettingsAsset> renderer_settings);

		~Renderer(void) { this->destroy(); }
		void destroy(void) override;

		void begin_frame(void) override;
		void end_frame(void) override;

		inline void bind_render_target(WeakRef<Graphics::RenderTarget> render_target) override { m_RenderTarget = render_target; }
		inline void unbind_render_target(void) override { m_RenderTarget = nullptr; }

		void begin_render_pass(const glm::vec4& clear_color = Colors::k_Black);
		void end_render_pass(void);

		void draw_imgui(void);

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

		void bind_vertex_buffer(
			View<const Graphics::Buffer> vertex_buffer,
			u64 offset = 0
		) override;

		void bind_index_buffer(
			View<const Graphics::Buffer> index_buffer,
			u64 offset = 0
		) override;

		void draw_vertices(
			u32 vertex_count,
			u32 instance_count = 1,
			u32 first_vertex = 0,
			u32 first_instance = 0
		) override;

		void draw_indexed(
			u32 index_count,
			u32 instance_count = 1,
			u32 first_index = 0,
			u32 first_instance = 0
		) override;

		void dispatch_compute(
			glm::uvec3 workgroup_count
		) override;

		[[nodiscard]] inline vk::CommandPool& graphics_cmd_pool(void) { return m_GraphicsCommandPool; }
		[[nodiscard]] inline const vk::CommandPool& graphics_cmd_pool(void) const { return m_GraphicsCommandPool; }

		[[nodiscard]] inline const vk::CommandBuffer& current_cmd_buffer(void) const { return m_CommandBuffers[m_FrameIndex]; }
		[[nodiscard]] inline vk::CommandBuffer& current_cmd_buffer(void) { return m_CommandBuffers[m_FrameIndex]; }

		[[nodiscard]] inline u32 current_frame_index(void) const override { return m_FrameIndex; }

		[[nodiscard]] inline vk::DescriptorPool& descriptor_pool(void) { return m_DescriptorPool; }
		[[nodiscard]] inline const vk::DescriptorPool& descriptor_pool(void) const { return m_DescriptorPool; }
		
		[[nodiscard]] inline Ref<const RendererSettingsAsset> settings(void) const override { return m_RendererSettings;  }

		[[nodiscard]] inline WeakRef<const Graphics::RenderTarget> current_render_target(void) const override { return m_RenderTarget; }


		[[nodiscard]] inline operator bool(void) const { return m_GraphicsCommandPool; }
	private:
		void _create_cmd_objects(void);
		void _create_descriptor_pool(void);
	private:
		WeakRef<Graphics::RenderTarget> m_RenderTarget = nullptr;

		vk::CommandPool m_GraphicsCommandPool = nullptr;

		ArrayList<vk::CommandBuffer> m_CommandBuffers;
		u32 m_FrameIndex = 0;
		
		vk::DescriptorPool m_DescriptorPool = nullptr;

		std::array<vk::DescriptorSet, 4> m_DescriptorSets{};
		std::array<u32, 32> m_DynamicOffsets{};

		Ref<const RendererSettingsAsset> m_RendererSettings;
	};
} // namespace Na

#endif // NA_VULKAN_IMPL_RENDERER_HPP