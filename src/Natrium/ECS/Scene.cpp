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
	FileErrorCode Scene::load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			return FileErrorCode::NotFound;
		}

		std::ifstream file(path);

		if (!file)
		{
			return FileErrorCode::Unknown;
		}

		nlohmann::json json;

		try
		{
			json = nlohmann::json::parse(file);
		} catch (const nlohmann::json::parse_error& e)
		{
			g_Logger.printf(Error, "Failed to parse scene file {}: {}", path.C_STR(), e.what());
			return FileErrorCode::InvalidFormat;
		}

		this->deserialize(json);

		return FileErrorCode::None;
	}

	FileErrorCode Scene::save(const std::filesystem::path& path)
	{
		std::ofstream file(path, std::ios::trunc);
		if (!file)
		{
			g_Logger.printf(Error, "Failed to save scene: {}", path.C_STR());
			return FileErrorCode::Unknown;
		}

		file << this->serialize().dump(4);
		file.close();

		return FileErrorCode::None;
	}

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
