#if !defined(NA_CAMERA3D_DATA_HPP)
#define NA_CAMERA3D_DATA_HPP

#include "Natrium/Core.hpp"

namespace Na {
	class Camera3dData {
	public:
		Camera3dData(void) = default;
		Camera3dData(
			const glm::vec3& pos,
			const glm::vec3& eye = glm::vec3(0.0f, 0.0f, 0.0f),
			float fov = 45.0f,
			glm::vec2 sensitivity = glm::vec2(0.1f, 0.1f)
		);

		~Camera3dData(void) = default;

		// vector is right, up, forward
		void move(const glm::vec3& amount);

		void rotate(glm::vec2 offset);
		void rotate_with_mouse(glm::vec2 mouse_pos);

		void on_mouse_capture(glm::vec2 mouse_pos);
		void on_mouse_release(void);

		[[nodiscard]] glm::mat4 calculate_view(void) const;
		[[nodiscard]] glm::mat4 calculate_projection(float aspect_ratio, float near_clip = 0.1f, float far_clip = 10.0f) const;

		inline void set_pos(const glm::vec3& pos) { m_Position = pos; }
		inline void set_eye(const glm::vec3& eye) { m_Position = eye; }

		inline void set_fov(float fov) { m_Fov = fov; }
		inline void set_sensitivity(glm::vec2 sensitivity) { m_Sensitivity = sensitivity; }

		inline void set_rotation(glm::vec2 rotation) { m_Rotation = rotation; }

		[[nodiscard]] inline const glm::vec3& pos(void) const { return m_Position; }
		[[nodiscard]] inline const glm::vec3& eye(void) const { return m_Eye; }

		[[nodiscard]] inline float fov(void) const { return m_Fov; }
		[[nodiscard]] inline glm::vec2 sensitivity(void) const { return m_Sensitivity; }

		[[nodiscard]] inline glm::vec2 rotation(void) const { return m_Rotation; }
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

#endif // NA_CAMERA3D_DATA_HPP