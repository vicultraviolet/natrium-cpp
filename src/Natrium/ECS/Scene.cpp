#include "Pch.hpp"
#include "Natrium/ECS/Scene.hpp"

namespace Na::ECS {
	nlohmann::json Scene::serialize(const RegistrySerializer& reg_serializer) const
	{
		nlohmann::json j;

		j["uuid"] = UUID::ToString(m_UUID);
		j["entities"] = reg_serializer.serialize(m_Registry);

		return j;
	}

	void Scene::deserialize(const RegistrySerializer& reg_serializer, const nlohmann::json& j)
	{
		m_UUID = UUID::FromString(j["uuid"]);
		m_Registry = reg_serializer.deserialize(j["entities"]);
	}
} // namespace Na::ECS
