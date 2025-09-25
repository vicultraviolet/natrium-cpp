#include "Pch.hpp"
#include "Natrium/ECS/Components/Transform3D.hpp"

namespace Na::ECS {
	Transform3D::Transform3D(void)
	: m_Position(0.0f, 0.0f, 0.0f),
	  m_Rotation(1.0f, 0.0f, 0.0f, 0.0f),
	  m_Scale(1.0f, 1.0f, 1.0f),
	  m_Matrix(1.0f)
	{

	}

	const glm::mat4& Transform3D::mat(void)
	{
		if (m_Dirty)
		{
			_calculate_matrix();
			m_Dirty = false;
		}

		return m_Matrix;
	}

	void Transform3D::_calculate_matrix(void)
	{
		m_Matrix = glm::mat4(1.0f);

		m_Matrix =  glm::translate(m_Matrix, m_Position);
		m_Matrix *= glm::mat4_cast(m_Rotation);
		m_Matrix =  glm::scale(m_Matrix, m_Scale);
	}

	void Transform3D::translate(const glm::vec3& translation)
	{
		m_Position += m_Rotation * translation;
		m_Dirty = true;
	}

	void Transform3D::rotate(const glm::quat& rot)
	{
		m_Rotation = rot * m_Rotation;
		m_Dirty = true;
	}

	void Transform3D::rotate(float radians, const glm::vec3& axis)
	{
		this->rotate(glm::angleAxis(radians, axis));
	}

	void Transform3D::scale_by(const glm::vec3& scaling)
	{
		m_Scale *= scaling;
		m_Dirty = true;
	}

	void Transform3D::scale_by(float uniform_scale)
	{
		this->scale_by(glm::vec3(uniform_scale));
	}

	glm::vec3 Transform3D::forward(void) const
	{
		return m_Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
	}

	glm::vec3 Transform3D::right(void) const
	{
		return m_Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
	}

	glm::vec3 Transform3D::up(void) const
	{
		return m_Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void Transform3D::look_at(const glm::vec3& target, const glm::vec3& world_up)
	{
		glm::vec3 direction = glm::normalize(target - m_Position);

		if (glm::length(direction) < 0.0001f)
			return;

		glm::mat4 mat = glm::lookAt(m_Position, target, world_up);
		m_Rotation = glm::quat_cast(glm::inverse(mat));

		m_Dirty = true;
	}

	nlohmann::json Transform3D::serialize(void) const
	{
		nlohmann::json j;

		j["position"] = nlohmann::json::array({ m_Position.x, m_Position.y, m_Position.z });
		j["rotation"] = nlohmann::json::array({ m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w });
		j["scale"] = nlohmann::json::array({ m_Scale.x, m_Scale.y, m_Scale.z });

		return j;
	}

	void Transform3D::deserialize(const nlohmann::json& j)
	{
		for (u64 i = 0; const auto& coord : j["position"])
			glm::value_ptr(m_Position)[i++] = coord.get<float>();

		for (u64 i = 0; const auto& coord : j["rotation"])
			glm::value_ptr(m_Rotation)[i++] = coord.get<float>();

		for (u64 i = 0; const auto& coord : j["scale"])
			glm::value_ptr(m_Scale)[i++] = coord.get<float>();

		m_Dirty = true;
	}
} // namespace Na::ECS 
