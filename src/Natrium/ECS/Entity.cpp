#include "Pch.hpp"
#include "Natrium/ECS/Entity.hpp"

namespace Na::ECS {
	Entity::Entity(void)
	: m_UUID(UUID::Generate())
	{

	}

	Entity::Entity(UUID_t uuid)
	: m_UUID(std::move(uuid))
	{

	}
} // namespace Na::ECS
