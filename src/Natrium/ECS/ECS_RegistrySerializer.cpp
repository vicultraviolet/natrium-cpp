#include "Pch.hpp"
#include "Natrium/ECS/ECS_RegistrySerializer.hpp"

namespace Na::ECS {
	nlohmann::json RegistrySerializer::serialize_entity(
		const Registry& r,
		const Entity& e
	) const
	{
		nlohmann::json j;

		for (const auto& [type_index, serializer] : m_Serializers)
		{
			nlohmann::json data = serializer(r, e);
			if (!data.is_null())
			{
				j.emplace(m_TypeIndexToName.at(type_index), std::move(data));
			}
		}

		return j;
	}

	void RegistrySerializer::deserialize_entity(
		const nlohmann::json& json,
		const Entity& e,
		Registry& r
	) const
	{
		for (const auto& [name, data] : json.items())
		{
			m_Deserializers.at(*m_NameToTypeInfo.at(name))(data, e, r);
		}
	}

	nlohmann::json RegistrySerializer::serialize(
		const Registry& r
	) const
	{
		nlohmann::json j;

		for (const Entity& e : r.entities())
		{
			j.emplace(
				UUID::ToString(e.uuid()),
				this->serialize_entity(r, e)
			);
		}

		return j;
	}

	void RegistrySerializer::deserialize(
		const nlohmann::json& json,
		Registry& r
	) const
	{
		for (const auto& [uuid, data] : json.items())
		{
			this->deserialize_entity(data, UUID::FromString(uuid), r);
		}
	}

	Registry RegistrySerializer::deserialize(const nlohmann::json& json) const
	{
		Registry r;
		this->deserialize(json, r);
		return r;
	}

} // namespace Na::ECS
