#if !defined(NA_ECS_REGISTRY_SERIALIZER_HPP)
#define NA_ECS_REGISTRY_SERIALIZER_HPP

#include "Natrium/ECS/ECS_Registry.hpp"

namespace Na::ECS {
	using SerializerFn = std::function<nlohmann::json(const Registry&, const Entity&)>;
	using DeserializerFn = std::function<void(const nlohmann::json&, const Entity&, Registry&)>;

	/*
		a single serialized entity looks like:
		{
			"pos": [0.2, 0.5, 1.0],
			"texture": "123-a"
		}

		a serialized registry looks like:
		{
			"1234-a": {
				"pos": [0.2, 0.5, 1.0],
				"texture": "123-a"
			},
			"1234-b": {
				"pos": [0.0, 0.2, 1.5],
				"hp": 5000
			}
		}
	*/

	class RegistrySerializer {
	public:
		void bind(void);
		void unbind(void);

		[[nodiscard]] inline static View<RegistrySerializer> Get(void) { return RegistrySerializer::s_Bound; }

		template<ComponentConcept T>
		void register_component(void)
		{
			const std::type_info& type = typeid(T);

			m_Serializers[type] = [](const Registry& r, const Entity& e) -> nlohmann::json
			{
				if (auto c = r.get_component<T>(e))
				{
					return c->serialize();
				}
				return nlohmann::json{};
			};

			m_Deserializers[type] = [](const nlohmann::json& j, const Entity& e, Registry& r) -> void
			{
				auto c = r.emplace_component<T>(e);
				c->deserialize(j);
			};
			m_TypeIndexToName[type] = T::GetTypeName();
			m_NameToTypeInfo[T::GetTypeName()] = &type;
		}

		[[nodiscard]] nlohmann::json serialize(const Registry& r) const;
		void deserialize(const nlohmann::json& json, Registry& r) const;

		Registry deserialize(const nlohmann::json& json) const;

		[[nodiscard]] nlohmann::json serialize_entity(const Registry& r, const Entity& e) const;
		void deserialize_entity(const nlohmann::json& json, const Entity& e, Registry& r) const;

	private:
		std::unordered_map<std::type_index, SerializerFn> m_Serializers;
		std::unordered_map<std::type_index, DeserializerFn> m_Deserializers;

		std::unordered_map<std::type_index, std::string_view> m_TypeIndexToName;
		std::unordered_map<std::string_view, const std::type_info*> m_NameToTypeInfo;

		static inline View<RegistrySerializer> s_Bound = nullptr;
	};
} // namespace Na::ECS

#endif // NA_ECS_REGISTRY_SERIALIZER_HPP