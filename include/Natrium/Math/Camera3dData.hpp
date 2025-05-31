#if !defined(NA_CAMERA3D_DATA_HPP)
#define NA_CAMERA3D_DATA_HPP

#include "Natrium/Core.hpp"

namespace Na {
	struct CameraMatrices {
		glm::mat4 view{ 1.0f };
		glm::mat4 proj{ 1.0f };
	};;

	class Camera3dData {
	public:
		Camera3dData(void) = default;
		Camera3dData(
			const glm::vec3& pos,
			const glm::vec3& eye = glm::vec3(0.0f, 0.0f, 0.0f),
			float fov = 45.0f,
			float aspect_ratio = 1.0f,
			float near_clip = 0.1f,
			float far_clip = 10.0f,
			glm::vec2 sensitivity = glm::vec2(0.1f, 0.1f)
		);

		~Camera3dData(void) = default;

		// vector is right, up, forward
		void move(const glm::vec3& amount);

		void rotate(glm::vec2 offset);
		void rotate_with_mouse(glm::vec2 mouse_pos);

		void on_mouse_capture(glm::vec2 mouse_pos);
		void on_mouse_release(void);

		// will recalculate matrices if dirty
		const glm::mat4& view_matrix(void);
		const glm::mat4& proj_matrix(void);
		const CameraMatrices& matrices(void);

		inline const glm::mat4& view_matrix(void) const { return m_ViewMatrix; }
		inline const glm::mat4& proj_matrix(void) const { return m_ProjectionMatrix; }
		inline const CameraMatrices& matrices(void) const { return m_CameraMatrices; }

		inline void set_pos(const glm::vec3& pos) { m_Position = pos; m_ViewMatrixDirty = true; }
		inline void set_eye(const glm::vec3& eye) { m_Position = eye; m_ViewMatrixDirty = true; }

		inline void set_fov(float fov) { m_Fov = fov; m_ProjectionMatrixDirty = true; }
		inline void set_aspect_ratio(float aspect_ratio) { m_AspectRatio = aspect_ratio; m_ProjectionMatrixDirty = true; }
		inline void set_near_clip(float near_clip) { m_NearClip = near_clip; m_ProjectionMatrixDirty = true; }
		inline void set_far_clip(float far_clip) { m_FarClip = far_clip; m_ProjectionMatrixDirty = true; }
		inline void set_clip_planes(float near, float far) { m_NearClip = near; m_FarClip = far; m_ProjectionMatrixDirty = true; }

		inline void set_sensitivity(glm::vec2 sensitivity) { m_Sensitivity = sensitivity; }

		[[nodiscard]] inline const glm::vec3& pos(void) const { return m_Position; }
		[[nodiscard]] inline const glm::vec3& eye(void) const { return m_Eye; }

		[[nodiscard]] inline float fov(void) const { return m_Fov; }
		[[nodiscard]] inline float aspect_ratio(void) const { return m_AspectRatio; }
		[[nodiscard]] inline float near_clip(void) const { return m_NearClip; }
		[[nodiscard]] inline float far_clip(void) const { return m_FarClip; }

		[[nodiscard]] inline glm::vec2 sensitivity(void) const { return m_Sensitivity; }

		[[nodiscard]] inline glm::vec2 rotation(void) const { return m_Rotation; }
	private:
		glm::vec3 m_Position{ 0.0f };
		glm::vec3 m_Eye{ 0.0f };

		float m_Fov = 45.0f;
		float m_AspectRatio = 1.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 10.0f;

		glm::vec2 m_Sensitivity{ 0.1f };

		glm::vec2 m_Rotation{ 0.0f }; // yaw, pitch

		glm::vec2 m_LastMousePosition{ 0.0f };
		bool m_FirstMouse = true;

		union {
			struct {
				glm::mat4 m_ViewMatrix;
				glm::mat4 m_ProjectionMatrix;
			};
			CameraMatrices m_CameraMatrices;
		};

		bool m_ViewMatrixDirty = true;
		bool m_ProjectionMatrixDirty = true;
	};
} // namespace Na

#endif // NA_CAMERA3D_DATA_HPP