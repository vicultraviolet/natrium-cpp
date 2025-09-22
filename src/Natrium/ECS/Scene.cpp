#include "Pch.hpp"
#include "Natrium/ECS/Scene.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na::ECS {
	nlohmann::json Scene::serialize(void) const
	{
		nlohmann::json j;

		j["uuid"] = UUID::ToString(m_UUID);
		j["entities"] = RegistrySerializer::Get()->serialize(m_Registry);

		return j;
	}

	void Scene::deserialize(const nlohmann::json& j)
	{
		m_UUID = UUID::FromString(j["uuid"]);
		m_Registry = RegistrySerializer::Get()->deserialize(j["entities"]);
	}
} // namespace Na::ECS
