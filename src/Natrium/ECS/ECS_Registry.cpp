#include "Pch.hpp"
#include "Natrium/ECS/ECS_Registry.hpp"

namespace Na::ECS {
	void Registry::_add_entity_if_unique(const Entity& e)
	{
		for (const Entity& _e : m_Entities)
			if (e == _e)
				return;

		m_Entities.emplace_back(e);
	}

} // namespace Na::ECS
