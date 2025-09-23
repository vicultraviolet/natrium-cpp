#if !defined(NA_ECS_REGISTRY_HPP)
#define NA_ECS_REGISTRY_HPP

#include "Natrium/ECS/ComponentContainer.hpp"

namespace Na::ECS {
	class Registry {
	public:
		template<ComponentConcept T>
		ComponentContainer<T>& get_container(void)
		{
			std::type_index type(typeid(T));

			if (m_Containers.find(type) == m_Containers.end())
			{
				m_Containers.try_emplace(
					type,
					MakeUnique<ComponentContainer<T>>()
				);
			} 

			View<IComponentContainer> container = m_Containers.at(type);
			return *static_ref_cast<ComponentContainer<T>>(container);
		}

		template<ComponentConcept T>
		const ComponentContainer<T>& get_container(void) const
		{
			View<IComponentContainer> container = m_Containers.at(typeid(T));
			return *static_ref_cast<ComponentContainer<T>>(container);
		}
		
		template<ComponentConcept T, typename... t_Args>
		View<T> emplace_component(const Entity& e, t_Args&&... __args)
		{
			this->_add_entity_if_unique(e);

			return this->get_container<T>().emplace(
				e,
				std::forward<t_Args>(__args)...
			);
		}

		template<ComponentConcept T>
		View<T> get_component(const Entity& e)
		{
			return this->get_container<T>().get(e);
		}

		template<ComponentConcept T>
		View<const T> get_component(const Entity& e) const
		{
			return this->get_container<T>().get(e);
		}

		template<ComponentConcept T>
		void remove_component(const Entity& e)
		{
			this->get_container<T>().remove_component(e);
		}

		template<ComponentConcept... t_Components>
		[[nodiscard]] ArrayList<Entity> get_entities_with(void) const
		{
			static_assert(sizeof...(t_Components) > 0, "Failed to get entities!: You must specify at least one component type!");

			std::unordered_set<Entity> candidates, intersection;

			bool first = true;

			((void)[&](void)
			{
				const auto& container = this->get_container<t_Components>();

				if (first)
				{
					first = false;

					for (const auto& [e, _] : container)
						candidates.insert(e);

					return;
				} 

				if (candidates.empty())
					return;

				for (const auto& [e, _] : container)
				{
					if (candidates.contains(e))
						intersection.insert(e);
				}
				candidates = std::move(intersection);
			}(), ...);

			return ArrayList<Entity>(candidates.begin(), candidates.end());
		}

		inline void clear(void) { m_Containers.clear(); m_Entities.destroy(); }

		[[nodiscard]] inline const ArrayList<Entity>& entities(void) const { return m_Entities; }
		[[nodiscard]] inline u64 entity_count(void) const { return m_Entities.size(); }
	private:
		void _add_entity_if_unique(const Entity& e);

	private:
		std::unordered_map<std::type_index, UniqueRef<IComponentContainer>> m_Containers;
		ArrayList<Entity> m_Entities{ 4 };
	};
} // namespace Na::ECS 

#endif // NA_ECS_REGISTRY_HPP