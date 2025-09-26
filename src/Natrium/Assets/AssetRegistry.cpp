#include "Pch.hpp"
#include "Natrium/Assets/AssetRegistry.hpp"

namespace Na {
	const UUID_t& AssetRegistry::get(const std::string& name) const
	{
		auto it = m_Registry.find(name);
		if (it != m_Registry.end())
			return it->second;

		return UUID::k_Null;
	}


	nlohmann::json AssetRegistry::serialize(void) const
	{
		nlohmann::json j;

		for (const auto& [name, id] : m_Registry)
			j.emplace(name, UUID::ToString(id));

		return j;
	}

	void AssetRegistry::deserialize(const nlohmann::json& j)
	{
		this->clear();

		for (const auto& [name, id] : j.items())
			m_Registry.try_emplace(name, UUID::FromString(id));
	}

	FileErrorCode AssetRegistry::_handle_missing(const std::filesystem::path& path)
	{
		g_Logger.printf(Info, "{} doesn't exist, creating empty file!", path.string());

		FileErrorCode code = this->save(path);

		if (code != FileErrorCode::None)
			return code;

		return FileErrorCode::None;
	}
} // namespace Na 
