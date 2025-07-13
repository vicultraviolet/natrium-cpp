#if !defined(NA_SHADER_HPP)
#define NA_SHADER_HPP

#include "Natrium/Graphics/Uniforms.hpp"

namespace Na::Graphics {
	enum class ShaderStage : u8 {
		None = 0,
		Vertex, Fragment,
		Compute,
		Geometry,
		TessellationControl, TessellationEvaluation
	};

	class Shader {
	public:
		using Uniforms = std::unordered_map<u32, View<const Uniform>>;
		
		virtual ~Shader(void) { this->destroy(); }
		virtual void destroy(void) { m_Uniforms.clear(); }

		Shader(const Shader& other) = delete;
		Shader& operator=(const Shader& other) = delete;

		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		void bind_uniform(u32 binding, View<const Uniform> uniform);
		[[nodiscard]] inline View<const Uniform> get_uniform(u32 binding) const { return m_Uniforms.find(binding) != m_Uniforms.end() ? m_Uniforms.at(binding) : nullptr; }

		inline void set_push_constant_size(u32 size) { m_PushConstantSize = size; }
		[[nodiscard]] inline u32 push_constant_size(void) const { return m_PushConstantSize; }

		[[nodiscard]] inline const Uniforms& uniforms(void) const { return m_Uniforms; }
		[[nodiscard]] inline Uniforms& uniforms(void) { return m_Uniforms; }

		[[nodiscard]] inline ShaderStage stage(void) const { return m_Stage; }
		[[nodiscard]] inline std::string_view entry_point(void) const { return m_EntryPoint; }

		[[nodiscard]] inline bool has_uniforms(void) const { return !m_Uniforms.empty(); }
		[[nodiscard]] inline u32 uniform_count(void) const { return m_UniformCount; }

		[[nodiscard]] inline bool has_push_constant(void) const { return m_PushConstantSize; }

		[[nodiscard]] inline operator bool(void) const { return m_Stage != ShaderStage::None; }
	protected:
		Shader(void) = default;

		Shader(ShaderStage stage, const std::string_view& entry_point = "main")
		: m_Stage(stage), m_EntryPoint(entry_point)
		{}
	private:
		Uniforms m_Uniforms;
		ShaderStage m_Stage = ShaderStage::None;
		u32 m_PushConstantSize = 0;
		u32 m_UniformCount = 0;
		std::string m_EntryPoint;
	};
} // namespace Na

#endif // NA_SHADER_HPP