#if !defined(NA_ECS_SYSTEM_HPP)
#define NA_ECS_SYSTEM_HPP

#include "Natrium/ECS/Component.hpp"
#include "Natrium/Core/Event.hpp"

#include "Natrium/ECS/ECS_Registry.hpp"

namespace Na::ECS {
	template<ComponentConcept... t_Components>
	class System {
	public:
		virtual void on_event(Event& e, View<t_Components>...) {}
		virtual void update(double dt, View<t_Components>...) {}
		virtual void draw(View<t_Components>...) {}
	};
} // namespace Na::ECS 

#endif // NA_ECS_SYSTEM_HPP