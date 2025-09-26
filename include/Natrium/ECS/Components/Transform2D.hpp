#if !defined(NA_ECS_COMPONENTS_TRANSFORM_2D_HPP)
#define NA_ECS_COMPONENTS_TRANSFORM_2D_HPP

#include "Natrium/ECS/Component.hpp"

namespace Na::ECS {
	class Transform2D : public Na::ECS::Component {
	public:
		Transform2D(void);

		[[nodiscard]] inline glm::vec2& position(void) { return m_Position; }
		[[nodiscard]] inline float&     rotation(void) { return m_Rotation; }
		[[nodiscard]] inline glm::vec2& scale(void)    { return m_Scale; }

		[[nodiscard]] inline const glm::vec2& position(void) const { return m_Position; }
		[[nodiscard]] inline       float      rotation(void) const { return m_Rotation; }
		[[nodiscard]] inline const glm::vec2& scale(void)    const { return m_Scale; }

		inline void set_position(const glm::vec2& pos) { m_Position = pos; m_Dirty = true; }
		inline void set_rotation(float radians) { m_Rotation = radians; m_Dirty = true; }
		inline void set_scale(const glm::vec3& scale) { m_Scale = scale; m_Dirty = true; }

		[[nodiscard]] const glm::mat3& mat(void);
		[[nodiscard]] inline const glm::mat3& mat(void) const { return m_Matrix; }

		void translate(const glm::vec2& translation);
		void rotate(float radians);
		void scale_by(const glm::vec2& scaling);

		[[nodiscard]] glm::vec2 forward(void) const;
		[[nodiscard]] glm::vec2 right(void) const;

		[[nodiscard]] void look_at(const glm::vec2& target);

		inline void mark_dirty(bool dirty = true) { m_Dirty = dirty; }

		nlohmann::json serialize(void) const override;
		void deserialize(const nlohmann::json& j) override;

		NA_DEFINE_COMPONENT_TYPE_NAME("transform2d");
	private:
		void _calculate_matrix(void);

	private:
		glm::vec2 m_Position;
		float     m_Rotation;
		glm::vec2 m_Scale;

		glm::mat3 m_Matrix;
		bool m_Dirty = true;
	};
} // namespace Na::ECS

#endif // NA_ECS_COMPONENTS_TRANSFORM_2D_HPP