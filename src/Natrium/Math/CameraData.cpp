#include "Pch.hpp"
#include "Natrium/Math/CameraData.hpp"

namespace Na {
	CameraData::CameraData(const glm::vec3& pos, const glm::vec3& eye, float fov, glm::vec2 sensitivity)
	: m_Position(pos), m_Eye(eye), m_Fov(fov), m_Sensitivity(sensitivity)
	{
		glm::vec3 dir = glm::normalize(eye - pos);
		m_Rotation.x = glm::degrees(atan2(dir.z, dir.x));
		m_Rotation.y = glm::degrees(asin(dir.y));
	}

	void CameraData::move(const glm::vec3& amount)
	{
		glm::vec3 forward = glm::normalize(m_Eye - m_Position);
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		glm::vec3 move = amount.x * right + amount.y * up + amount.z * forward;

		m_Position += move;
		m_Eye += move;
	}

	void CameraData::rotate(glm::vec2 offset)
	{
		offset *= m_Sensitivity;
		m_Rotation += offset;

		if (m_Rotation.y > 89.0f)
			m_Rotation.y = 89.0f;

		if (m_Rotation.y < -89.0f)
			m_Rotation.y = -89.0f;

		glm::vec3 direction(
			cos(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y)),
			sin(glm::radians(m_Rotation.y)),
			sin(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y))
		);
		m_Eye = m_Position + glm::normalize(direction);
	}

	void CameraData::rotate_with_mouse(glm::vec2 mouse_pos)
	{
		if (m_FirstMouse)
		{
			m_LastMousePosition = mouse_pos;
			m_FirstMouse = false;
		}
		
		glm::vec2 offset(mouse_pos.x - m_LastMousePosition.x, m_LastMousePosition.y - mouse_pos.y);
		this->rotate(offset);
		m_LastMousePosition = mouse_pos;
	}

	void CameraData::on_mouse_capture(glm::vec2 mouse_pos)
	{
		m_LastMousePosition = mouse_pos;
		m_FirstMouse = false;
	}

	void CameraData::on_mouse_release(void)
	{
		m_FirstMouse = true;
	}

	glm::mat4 CameraData::calculate_view(void) const
	{
		return glm::lookAt(m_Position, m_Eye, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 CameraData::calculate_projection(float aspect_ratio, float near_clip, float far_clip) const
	{
		return glm::perspectiveZO(glm::radians(m_Fov), aspect_ratio, near_clip, far_clip);
	}
} // namespace Na	
