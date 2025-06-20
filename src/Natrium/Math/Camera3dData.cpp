#include "Pch.hpp"
#include "Natrium/Math/Camera3dData.hpp"

namespace Na {
	static PushConstant pushConstantData{
		.shader_stage = ShaderStageBits::Vertex,
		.size = sizeof(glm::mat4) * 2,
		.offset = 0
	};

	[[nodiscard]] const PushConstant& Camera3dData::PushConstantData(void)
	{
		return pushConstantData;
	}

	Camera3dData::Camera3dData(
		const glm::vec3& pos,
		const glm::vec3& eye,
		float fov,
		float aspect_ratio,
		float near_clip,
		float far_clip,
		glm::vec2 sensitivity
	)
	: m_Position(pos),
	m_Eye(eye),
	m_Fov(fov),
	m_AspectRatio(aspect_ratio),
	m_NearClip(near_clip),
	m_FarClip(far_clip),
	m_Sensitivity(sensitivity)
	{
		glm::vec3 dir = glm::normalize(eye - pos);
		m_Rotation.x = glm::degrees(atan2(dir.z, dir.x));
		m_Rotation.y = glm::degrees(asin(dir.y));
	}

	void Camera3dData::move(const glm::vec3& amount)
	{
		glm::vec3 forward = glm::normalize(m_Eye - m_Position);
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		glm::vec3 move = amount.x * right + amount.y * up + amount.z * forward;

		m_Position += move;
		m_Eye += move;

		m_ViewMatrixDirty = true;
	}

	void Camera3dData::rotate(glm::vec2 offset)
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

		m_ViewMatrixDirty = true;
	}

	void Camera3dData::rotate_with_mouse(glm::vec2 mouse_pos)
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

	void Camera3dData::on_mouse_capture(glm::vec2 mouse_pos)
	{
		m_LastMousePosition = mouse_pos;
		m_FirstMouse = false;
	}

	void Camera3dData::on_mouse_release(void)
	{
		m_FirstMouse = true;
	}

	const glm::mat4& Camera3dData::view_matrix(void)
	{
		if (m_ViewMatrixDirty)
		{
			m_Matrices.view = glm::lookAt(m_Position, m_Eye, glm::vec3(0.0f, 1.0f, 0.0f));
			m_ViewMatrixDirty = false;
		}
		return m_Matrices.view;
	}

	const glm::mat4& Camera3dData::proj_matrix(void)
	{
		if (m_ProjectionMatrixDirty)
		{
			m_Matrices.proj = glm::perspectiveZO(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
			m_ProjectionMatrixDirty = false;
		}
		return m_Matrices.proj;
	}

	const CameraMatrices& Camera3dData::matrices(void)
	{
		if (m_ViewMatrixDirty)
		{
			m_Matrices.view = glm::lookAt(m_Position, m_Eye, glm::vec3(0.0f, 1.0f, 0.0f));
			m_ViewMatrixDirty = false;
		}
		if (m_ProjectionMatrixDirty)
		{
			m_Matrices.proj = glm::perspectiveZO(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
			m_ProjectionMatrixDirty = false;
		}
		return m_Matrices;
	}
} // namespace Na	
