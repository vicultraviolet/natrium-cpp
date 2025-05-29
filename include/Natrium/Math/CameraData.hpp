#if !defined(NA_CAMERA_DATA_HPP)
#define NA_CAMERA_DATA_HPP

#include "Natrium/Core.hpp"

namespace Na {
	class CameraData {
	public:
		CameraData(void) = default;
		CameraData(
			const glm::vec3& pos,
			const glm::vec3& eye = glm::vec3(0.0f, 0.0f, 0.0f),
			float fov = 45.0f,
			glm::vec2 sensitivity = glm::vec2(0.1f, 0.1f)
		);

		~CameraData(void) = default;

		// vector is right, up, forward
		void move(const glm::vec3& amount);

		void rotate(glm::vec2 offset);
		void rotate_with_mouse(glm::vec2 mouse_pos);

		void on_mouse_capture(glm::vec2 mouse_pos);
		void on_mouse_release(void);

		[[nodiscard]] glm::mat4 calculate_view(void) const;
		[[nodiscard]] glm::mat4 calculate_projection(float aspect_ratio, float near_clip = 0.1f, float far_clip = 10.0f) const;

		[[nodiscard]] inline const glm::vec3& pos(void) const { return m_Position; }
		[[nodiscard]] inline const glm::vec3& eye(void) const { return m_Eye; }

		[[nodiscard]] inline float fov(void) const { return m_Fov; }
		[[nodiscard]] inline glm::vec2 sensitivity(void) const { return m_Sensitivity; }

		[[nodiscard]] inline glm::vec2 rotation(void) const { return m_Rotation; }

		[[nodiscard]] inline glm::vec2 last_mouse_pos(void) const { return m_LastMousePosition; }
		[[nodiscard]] inline bool is_first_mouse(void) const { return m_FirstMouse; }
	private:
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Eye{ 0.0f, 0.0f, 0.0f };

		float m_Fov = 45.0f;
		glm::vec2 m_Sensitivity{ 0.1f, 0.1f };

		glm::vec2 m_Rotation{ 0.0f, 0.0f }; // yaw, pitch

		glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };
		bool m_FirstMouse = true;
	};
} // namespace Na

#endif // NA_CAMERA_DATA_HPP