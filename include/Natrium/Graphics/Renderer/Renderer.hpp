#if !defined(NA_RENDERER_HPP)
#define NA_RENDERER_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/Renderer/RendererCore.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

#include "Natrium/Graphics/Buffers/VertexBuffer.hpp"
#include "Natrium/Graphics/Buffers/IndexBuffer.hpp"
#include "Natrium/Graphics/Buffers/UniformBuffer.hpp"
#include "Natrium/Graphics/Buffers/StorageBuffer.hpp"

namespace Na {
	struct FrameData {
		bool              valid = false;

		vk::CommandBuffer cmd_buffer;

		vk::Semaphore     image_available_semaphore;
		vk::Semaphore     render_finished_semaphore;
		vk::Fence         in_flight_fence;
	};

	class Renderer {
	public:
		Renderer(void) = default;
		Renderer(Window& window, AssetHandle<RendererSettings> settings);

		void destroy(void);
		inline ~Renderer(void) { this->destroy(); }

		[[nodiscard]] bool begin_frame(const glm::vec4& color = Colors::k_Black);
		void end_frame(void);

		void bind_pipeline(const GraphicsPipeline& pipeline);
		void set_push_constant(const PushConstant& push_constant, const void* data, const GraphicsPipeline& pipeline);

		void draw_vertices(const VertexBuffer& vertex_buffer, u32 vertex_count, u32 instance_count = 1);
		void draw_indexed(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer, u32 instance_count = 1);

		void set_descriptor_buffer(void* buffer, const void* data) const;
		template<typename T>
		inline void set_descriptor_buffer(const T& buffer, const void* data) const { this->set_descriptor_buffer((void*)&buffer, data); }

		[[nodiscard]] inline AssetHandle<RendererSettings> settings(void) { return m_Core.settings(); }

		[[nodiscard]] inline RendererCore& core(void) { return m_Core; }
		[[nodiscard]] inline const RendererCore& core(void) const { return m_Core; }

		[[nodiscard]] inline FrameData& current_frame(void) { return m_Frames[m_FrameIndex]; }
		[[nodiscard]] inline const FrameData& current_frame(void) const { return m_Frames[m_FrameIndex]; }

		[[nodiscard]] inline u32 current_frame_index(void) const { return m_FrameIndex; }

		[[nodiscard]] inline operator bool(void) const { return m_Core; }

		Renderer(const Renderer& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;

		Renderer(Renderer&& other);
		Renderer& operator=(Renderer&& other);
	private:
		void _create_command_objects(void);
		void _create_sync_objects(void);
	private:
		RendererCore m_Core;

		vk::CommandPool m_GraphicsCmdPool;

		ArrayList<FrameData> m_Frames;
		u32 m_FrameIndex = 0;

		ArrayList<vk::Fence> m_ImageInFlightFences;
		u32 m_ImageIndex = 0;
	};
} // namespace Na

#endif // NA_RENDERER_HPP