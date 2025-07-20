#include "Pch.hpp"
#include "Natrium/Graphics/Shader.hpp"

namespace Na::Graphics {
	Shader::Shader(Shader&& other) noexcept
	: m_Stage(std::exchange(other.m_Stage, ShaderStage::None)),
	  m_EntryPoint(std::move(other.m_EntryPoint))
	{}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_Stage = std::exchange(other.m_Stage, ShaderStage::None);
		m_EntryPoint = std::move(other.m_EntryPoint);

		return *this;
	}
} // namespace Na::Graphics
