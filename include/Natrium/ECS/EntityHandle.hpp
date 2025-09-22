#if !defined(NA_ENTITY_HANDLE_HPP)
#define NA_ENTITY_HANDLE_HPP

#include "Natrium/ECS/Entity.hpp"
#include "Natrium/ECS/Scene.hpp"

namespace Na::ECS {
	class EntityHandle {
	public:
		Entity e;
		WeakRef<Scene> scene;

		EntityHandle(WeakRef<Scene> scene) : scene(scene) {}

		template<ComponentConcept T, typename... t_Args>
		View<T> emplace_component(t_Args&&... __args)
		{
			if (auto s = scene.lock())
				return s->registry().emplace_component<T>(e, std::forward<t_Args>(__args)...);

			return nullptr;
		}

		template<ComponentConcept T>
		void remove_component(void)
		{
			if (auto s = scene.lock())
				s->registry().remove_component<T>(e);
		}

		template<ComponentConcept T>
		[[nodiscard]] View<T> get_component(void)
		{
			if (auto s = scene.lock())
				return s->registry().get_component<T>(e);

			return nullptr;
		}

		[[nodiscard]] inline operator bool(void) const { return e && scene; }
	};
} // namespace Na::ECS

#endif // NA_ENTITY_HANDLE_HPP