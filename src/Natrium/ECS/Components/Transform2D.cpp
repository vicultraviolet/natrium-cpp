#include "Pch.hpp"
#include "Natrium/ECS/Components/Transform2D.hpp"

#include <glm/gtx/rotate_vector.hpp>

namespace Na::ECS {
	Transform2D::Transform2D(void)
	: m_Position(0.0f, 0.0f),
	  m_Rotation(0.0f),
	  m_Scale(1.0f, 1.0f),
	  m_Matrix(1.0f)
	{
	
	}

	const glm::mat3& Transform2D::mat(void)
	{
		if (m_Dirty)
		{
			_calculate_matrix();
			m_Dirty = false;
		}

		return m_Matrix;
	}

	void Transform2D::_calculate_matrix(void)
	{
		m_Matrix = glm::mat3(1.0f);

		m_Matrix = glm::translate(m_Matrix, m_Position);
		m_Matrix = glm::scale(m_Matrix, m_Scale);
		m_Matrix = glm::rotate(m_Matrix, m_Rotation);
	}

	void Transform2D::translate(const glm::vec2& translation)
	{
		m_Position += glm::rotate(translation, m_Rotation);
		m_Dirty = true;
	}

	void Transform2D::rotate(float radians)
	{
		m_Rotation += radians;
		m_Dirty = true;
	}

	void Transform2D::scale_by(const glm::vec2& scaling)
	{
		m_Scale *= scaling;
		m_Dirty = true;
	}

	glm::vec2 Transform2D::forward(void) const
	{
		return glm::vec2(glm::cos(m_Rotation), glm::sin(m_Rotation));
	}

	glm::vec2 Transform2D::right(void) const
	{
		return glm::vec2(-glm::sin(m_Rotation), glm::cos(m_Rotation));
	}

	void Transform2D::look_at(const glm::vec2& target)
	{
		glm::vec2 direction = glm::normalize(target - m_Position);
		m_Rotation = atan2(direction.y, direction.x);

		m_Dirty = true;
	}

	nlohmann::json Transform2D::serialize(void) const
	{
		nlohmann::json j;

		j["position"] = nlohmann::json::array({ m_Position.x, m_Position.y });
		j["rotation"] = m_Rotation;
		j["scale"]    = nlohmann::json::array({ m_Scale.x, m_Scale.y });

		return j;
	}

	void Transform2D::deserialize(const nlohmann::json& j)
	{
		for (u64 i = 0; const auto& coord : j["position"])
			glm::value_ptr(m_Position)[i++] = coord.get<float>();

		m_Rotation = j["rotation"].get<float>();

		for (u64 i = 0; const auto& coord : j["scale"])
			glm::value_ptr(m_Scale)[i++] = coord.get<float>();

		m_Dirty = true;
	}
} // namespace Na::ECS 
