#include "Pch.hpp"
#include "Natrium/Graphics/Shader.hpp"

namespace Na::Graphics {
	Shader::Shader(Shader&& other) noexcept
	: m_Uniforms(std::move(other.m_Uniforms)),
	  m_Stage(std::exchange(other.m_Stage, ShaderStage::None)),
	  m_EntryPoint(std::move(other.m_EntryPoint))
	{}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_Uniforms = std::move(other.m_Uniforms);
		m_Stage = std::exchange(other.m_Stage, ShaderStage::None);
		m_EntryPoint = std::move(other.m_EntryPoint);

		return *this;
	}

	void Shader::bind_uniform(u32 binding, View<const Uniform> uniform)
	{
		if (!uniform)
		{
			m_Uniforms.erase(binding);
			return;
		}

		m_Uniforms[binding] = uniform;
		if (uniform->type() != UniformType::None)
			m_UniformCount++;
	}
} // namespace Na::Graphics
