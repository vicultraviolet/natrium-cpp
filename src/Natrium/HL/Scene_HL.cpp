#include "Pch.hpp"
#include "Natrium/HL/Scene_HL.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na::HL {
	nlohmann::json Scene::serialize(void) const
	{
		nlohmann::json j;

		j["uuid"] = UUID::ToString(m_UUID);
		j["entities"] = ECS::RegistrySerializer::Get()->serialize(m_Registry);

		return j;
	}

	void Scene::deserialize(const nlohmann::json& j)
	{
		m_UUID = UUID::FromString(j["uuid"]);
		m_Registry = ECS::RegistrySerializer::Get()->deserialize(j["entities"]);
	}
} // namespace Na::ECS
