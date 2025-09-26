#if !defined(NA_ECS_COMPONENT_CONTAINER_HPP)
#define NA_ECS_COMPONENT_CONTAINER_HPP

#include "Natrium/ECS/Entity.hpp"
#include "Natrium/ECS/Component.hpp"

namespace Na::ECS {
    class IComponentContainer {
    public:
        virtual ~IComponentContainer(void) = default;

        [[nodiscard]] virtual bool empty(void) const = 0;
    };

	template<ComponentConcept T>
	class ComponentContainer : public IComponentContainer {
	public:
        using T_t = T;

        using Components = std::unordered_map<Entity, T>;

        using iterator = Components::iterator;
        using const_iterator = Components::const_iterator;

        ComponentContainer(void) = default;
        ~ComponentContainer(void) = default;

        template<typename... t_Args>
        View<T> emplace(const Entity& e, t_Args&&... args)
        {
            auto [it, inserted] = m_Components.try_emplace(
                e,
                std::forward<t_Args>(args)...
            );
            return &it->second;
        }

        [[nodiscard]] View<T> get(const Entity& e)
        {
            auto it = m_Components.find(e);
            return (it != m_Components.end()) ? &it->second : nullptr;
        }

        [[nodiscard]] View<const T> get(const Entity& e) const
        {
            auto it = m_Components.find(e);
            return (it != m_Components.end()) ? &it->second : nullptr;
        }

        inline void remove(const Entity &e) { m_Components.erase(e); }

        inline void clear(void) { m_Components.clear(); }

        [[nodiscard]] inline iterator begin(void) { return m_Components.begin(); }
        [[nodiscard]] inline const_iterator begin(void) const { return m_Components.begin(); }
        [[nodiscard]] inline const_iterator cbegin(void) const { return m_Components.cbegin(); }

        [[nodiscard]] inline iterator end(void) { return m_Components.end(); }
        [[nodiscard]] inline const_iterator end(void) const { return m_Components.end(); }
        [[nodiscard]] inline const_iterator bend(void) const { return m_Components.bend(); }

        [[nodiscard]] inline u64 size(void) const { return m_Components.size(); }
        [[nodiscard]] inline bool empty(void) const override { return m_Components.empty(); }
	private:
        Components m_Components;
	};
} // namespace Na::ECS

#endif // NA_ECS_COMPONENT_CONTAINER_HPP