#if !defined(NA_ECS_SYSTEM_MANAGER_HPP)
#define NA_ECS_SYSTEM_MANAGER_HPP

#include "Natrium/ECS/ECS_System.hpp"
#include "Natrium/ECS/ECS_Registry.hpp"

namespace Na::ECS {
	class SystemManager {
	public:
		View<Registry> registry;

		template<ComponentConcept... t_Components>
		void on_event(System<t_Components...>& system, Event& event)
		{
			for (const Entity& e : registry->get_entities_with<t_Components...>())
				system.on_event(event, registry->get_component<t_Components>(e)...);
		}

		template<ComponentConcept... t_Components>
		void update(System<t_Components...>& system, double dt)
		{
			for (const Entity& e : registry->get_entities_with<t_Components...>())
				system.update(dt, registry->get_component<t_Components>(e)...);
		}

		template<ComponentConcept... t_Components>
		void draw(System<t_Components...>& system)
		{
			for (const Entity& e : registry->get_entities_with<t_Components...>())
				system.draw(registry->get_component<t_Components>(e)...);
		}
	};
} // namespace Na::ECS

#endif // NA_ECS_SYSTEM_MANAGER_HPP