#if !defined(NA_ECS_ENTITY_HPP)
#define NA_ECS_ENTITY_HPP

#include "Natrium/Core/UUID.hpp"

namespace Na::ECS {
	class Entity {
	public:
		Entity(void);
		Entity(UUID_t uuid);

		[[nodiscard]] inline const UUID_t& uuid(void) const { return m_UUID; }

		[[nodiscard]] inline operator bool(void) const { return !m_UUID.is_nil(); }
		[[nodiscard]] inline bool operator==(const Entity& other) const { return m_UUID == other.m_UUID; }
	private:
		UUID_t m_UUID;
	};
} // namespace Na::ECS

namespace std {
	template<>
	struct hash<Na::ECS::Entity> {
		[[nodiscard]] inline size_t operator()(const Na::ECS::Entity& e) const
		{
			return std::hash<Na::UUID_t>{}(e.uuid());
		}
	};
} // namespace std

#endif // NA_ECS_ENTITY_HPP