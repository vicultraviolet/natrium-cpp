#if !defined(NA_RENDERER_HPP)
#define NA_RENDERER_HPP

#include "Natrium/Core/Window.hpp"
#include "Natrium/Graphics/Colors.hpp"
#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/Buffers.hpp"
#include "Natrium/Graphics/Uniforms.hpp"
#include "Natrium/Graphics/UniformSet.hpp"
#include "Natrium/Assets/RendererSettingsAsset.hpp"
#include "Natrium/Graphics/Shader.hpp"

namespace Na::Graphics {
	class Pipeline;

	class Renderer {
	public:
		[[nodiscard]] static UniqueRef<Renderer> Make(
			const Window& window,
			Ref<const RendererSettingsAsset> settings = nullptr
		);

		virtual ~Renderer(void) { this->destroy(); }
		virtual void destroy(void) {}

		[[nodiscard]] virtual bool begin_frame(const glm::vec4& color = Colors::k_Black) = 0;
		virtual void end_frame(void) = 0;

		virtual void bind_pipeline(View<const Pipeline> pipeline) = 0;

		virtual void bind_uniform_set(
			View<const UniformSet> uniform_set,
			View<const Pipeline> pipeline,
			u32 set_index = 0
		) = 0;

		virtual void bind_uniform_sets(
			const View<const UniformSet>* uniform_sets,
			u64 set_count,
			View<const Pipeline> pipeline,
			u32 starting_index = 0
		) = 0;

		void bind_uniform_sets(
			const std::initializer_list<View<const UniformSet>>& uniform_sets,
			View<const Pipeline> pipeline,
			u32 starting_index = 0
		);

		virtual void set_push_constant(
			u32 size,
			ShaderStage stage,
			u32 offset,
			const void* data,
			View<const Pipeline> pipeline
		) const = 0;

		virtual void draw_vertices(
			View<const VertexBuffer> vertex_buffer,
			u32 verex_count,
			u32 instance_count = 1,
			u32 first_vertex = 0,
			u32 first_instance = 0
		) = 0;

		virtual void draw_indexed(
			View<const VertexBuffer> vertex_buffer,
			View<const IndexBuffer> index_buffer,
			u32 instance_count = 1,
			u32 first_index = 0,
			u32 first_instance = 0
		) = 0;

		virtual void dispatch_compute(
			glm::uvec3 workgroup_count
		) = 0;

		virtual void set_descriptor_buffer(
			View<const Uniform> buffer,
			const void* data
		) const = 0;

		[[nodiscard]] virtual const Window& window(void) const = 0;
		[[nodiscard]] virtual Ref<const RendererSettingsAsset> settings(void) const = 0;
		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_RENDERER_HPP