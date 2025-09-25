#if !defined(NA_ECS_COMPONENTS_TRANSFORM_3D_HPP)
#define NA_ECS_COMPONENTS_TRANSFORM_3D_HPP

#include "Natrium/ECS/Component.hpp"

namespace Na::ECS {
	class Transform3D : public Na::ECS::Component {
	public:
		Transform3D(void);

		[[nodiscard]] inline glm::vec3& position(void) { return m_Position; }
		[[nodiscard]] inline glm::quat& rotation(void) { return m_Rotation; }
		[[nodiscard]] inline glm::vec3& scale(void)    { return m_Scale; }

		[[nodiscard]] inline const glm::vec3& position(void) const { return m_Position; }
		[[nodiscard]] inline const glm::quat& rotation(void) const { return m_Rotation; }
		[[nodiscard]] inline const glm::vec3& scale(void) const    { return m_Scale; }

		inline void set_position(const glm::vec3& pos) { m_Position = pos; m_Dirty = true; }
		inline void set_rotation(const glm::quat& rot) { m_Rotation = rot; m_Dirty = true; }
		inline void set_scale(const glm::vec3& scale) { m_Scale = scale;  m_Dirty = true; }
		inline void set_uniform_scale(float scale) { this->set_scale(glm::vec3(scale)); }

		[[nodiscard]] const glm::mat4& mat(void);
		[[nodiscard]] inline const glm::mat4& mat(void) const { return m_Matrix; }

		void translate(const glm::vec3& translation);

		void rotate(const glm::quat& rot);
		void rotate(float radians, const glm::vec3& axis);

		void scale_by(const glm::vec3& scaling);
		void scale_by(float scaling);

		[[nodiscard]] glm::vec3 forward(void) const;
		[[nodiscard]] glm::vec3 right(void) const;
		[[nodiscard]] glm::vec3 up(void) const;

		void look_at(
			const glm::vec3& target,
			const glm::vec3& world_up = glm::vec3(0.0f, 1.0f, 0.0f)
		);

		nlohmann::json serialize(void) const override;
		void deserialize(const nlohmann::json& j) override;

		NA_DEFINE_COMPONENT_TYPE_NAME("transform3d");
	private:
		void _calculate_matrix(void);

	private:
		glm::vec3 m_Position;
		glm::quat m_Rotation;
		glm::vec3 m_Scale;

		glm::mat4 m_Matrix;
		bool m_Dirty = true;
	};
} // namespace Na::ECS

#endif // NA_ECS_COMPONENTS_TRANSFORM_3D_HPP