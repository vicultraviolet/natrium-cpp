#if !defined(NA_RENDERER_HPP)
#define NA_RENDERER_HPP

#include "Natrium/Core/Window.hpp"
#include "Natrium/Graphics/Colors.hpp"
#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/Buffer.hpp"
#include "Natrium/Graphics/UniformSet.hpp"
#include "Natrium/Assets/RendererSettingsAsset.hpp"
#include "Natrium/Graphics/Shader.hpp"
#include "Natrium/Graphics/RenderTargets.hpp"

namespace Na::Graphics {
	class Pipeline;

	class Renderer {
	public:
		[[nodiscard]] static UniqueRef<Renderer> Make(Ref<const RendererSettingsAsset> settings);

		virtual ~Renderer(void) { this->destroy(); }
		virtual void destroy(void) {}

		virtual void begin_frame(void) = 0;
		virtual void end_frame(void) = 0;

		virtual void bind_render_target(WeakRef<RenderTarget> render_target) = 0;
		virtual void unbind_render_target(void) = 0;

		virtual void begin_render_pass(const glm::vec4& clear_color = Colors::k_Black) = 0;
		virtual void end_render_pass(void) = 0;

		virtual void draw_imgui(void) = 0;

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
			View<const Buffer> vertex_buffer,
			u32 verex_count,
			u32 instance_count = 1,
			u32 first_vertex = 0,
			u32 first_instance = 0
		) = 0;

		virtual void draw_indexed(
			View<const Buffer> vertex_buffer,
			View<const Buffer> index_buffer,
			u32 index_count,
			u32 instance_count = 1,
			u32 first_index = 0,
			u32 first_instance = 0
		) = 0;

		virtual void dispatch_compute(
			glm::uvec3 workgroup_count
		) = 0;

		[[nodiscard]] virtual u32 current_frame_index(void) const = 0;

		[[nodiscard]] virtual Ref<const RendererSettingsAsset> settings(void) const = 0;

		[[nodiscard]] virtual WeakRef<const RenderTarget> current_render_target(void) const = 0;

		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_RENDERER_HPP