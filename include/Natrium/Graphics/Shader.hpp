#if !defined(NA_SHADER_HPP)
#define NA_SHADER_HPP

#include "Natrium/Core.hpp"

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
		virtual ~Shader(void) { this->destroy(); }
		virtual void destroy(void) { }

		Shader(const Shader& other) = delete;
		Shader& operator=(const Shader& other) = delete;

		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		inline void set_push_constant_size(u32 size) { m_PushConstantSize = size; }
		[[nodiscard]] inline u32 push_constant_size(void) const { return m_PushConstantSize; }

		[[nodiscard]] inline ShaderStage stage(void) const { return m_Stage; }
		[[nodiscard]] inline std::string_view entry_point(void) const { return m_EntryPoint; }

		[[nodiscard]] inline bool has_push_constant(void) const { return m_PushConstantSize; }

		[[nodiscard]] inline operator bool(void) const { return m_Stage != ShaderStage::None; }
	protected:
		Shader(void) = default;

		Shader(ShaderStage stage, const std::string_view& entry_point = "main")
		: m_Stage(stage), m_EntryPoint(entry_point)
		{}
	private:
		ShaderStage m_Stage = ShaderStage::None;
		u32 m_PushConstantSize = 0;
		std::string m_EntryPoint;
	};
} // namespace Na

#endif // NA_SHADER_HPP