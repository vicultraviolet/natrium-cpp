#if !defined(NA_ECS_REGISTRY_HPP)
#define NA_ECS_REGISTRY_HPP

#include "Natrium/ECS/ComponentContainer.hpp"

namespace Na::ECS {
	class Registry {
	public:
		template<ComponentConcept T>
		ComponentContainer<T>& storage(void)
		{
			std::type_index type(typeid(T));

			if (m_Storages.find(type) == m_Storages.end())
			{
				m_Storages.try_emplace(
					type,
					MakeUnique<ComponentContainer<T>>()
				);
			} 

			View<IComponentContainer> storage = m_Storages.at(type);
			return *static_ref_cast<ComponentContainer<T>>(storage);
		}

		template<ComponentConcept T>
		const ComponentContainer<T>& storage(void) const
		{
			View<IComponentContainer> storage = m_Storages.at(typeid(T));
			return *static_ref_cast<ComponentContainer<T>>(storage);
		}
		
		template<ComponentConcept T, typename... t_Args>
		View<T> emplace_component(const Entity& e, t_Args&&... __args)
		{
			this->_add_entity_if_unique(e);

			return this->storage<T>().emplace(
				e,
				std::forward<t_Args>(__args)...
			);
		}

		template<ComponentConcept T>
		View<T> get_component(const Entity& e)
		{
			return this->storage<T>().get(e);
		}

		template<ComponentConcept T>
		View<const T> get_component(const Entity& e) const
		{
			return this->storage<T>().get(e);
		}

		template<ComponentConcept T>
		void remove_component(const Entity& e)
		{
			this->storage<T>().remove_component(e);
		}

		template<ComponentConcept... t_Components>
		[[nodiscard]] ArrayList<Entity> get_entities_with(void) const
		{
			static_assert(sizeof...(t_Components) > 0, "Failed to get entities!: You must specify at least one component type!");

			std::unordered_set<Entity> candidates, intersection;

			bool first = true;

			((void)[&](void)
			{
				const auto& container = this->storage<t_Components>();

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

		inline void clear(void) { m_Storages.clear(); m_Entities.destroy(); }

		[[nodiscard]] inline const ArrayList<Entity>& entities(void) const { return m_Entities; }
		[[nodiscard]] inline u64 entity_count(void) const { return m_Entities.size(); }
	private:
		void _add_entity_if_unique(const Entity& e);

	private:
		std::unordered_map<std::type_index, UniqueRef<IComponentContainer>> m_Storages;
		ArrayList<Entity> m_Entities{ 4 };
	};
} // namespace Na::ECS 

#endif // NA_ECS_REGISTRY_HPP