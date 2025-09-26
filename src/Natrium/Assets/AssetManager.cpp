#include "Pch.hpp"
#include "Natrium/Assets/AssetManager.hpp"

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"
#include "Natrium/Assets/TextAsset.hpp"

namespace Na {
	AssetManager::AssetManager(const AssetManagerCreateInfo& info)
	: m_EngineAssetsDirectory(info.engine_assets_dir),
	  m_ShaderOutputDirectory(info.shader_output_dir),
	  m_AssetRegistryPath(info.asset_registry_path)
	{
		m_Registry.load(info.asset_registry_path);

		if (!std::filesystem::exists(m_EngineAssetsDirectory))
			std::filesystem::create_directories(m_EngineAssetsDirectory);

		if (!std::filesystem::exists(m_ShaderOutputDirectory))
			std::filesystem::create_directories(m_ShaderOutputDirectory);
	}

	void AssetManager::destroy(void)
	{
		this->unbind();

		m_ShaderOutputDirectory.clear();
		m_EngineAssetsDirectory.clear();
		m_Assets.clear();
		m_Registry.clear();
	}

	void AssetManager::bind(void)
	{
		AssetManager::s_Bound = this;
	}

	void AssetManager::unbind(void)
	{
		if (AssetManager::s_Bound == this)
			AssetManager::s_Bound = nullptr;
	}

	void AssetManager::destroy_asset(const UUID_t& uuid)
	{
		m_Assets.erase(uuid);
	}

	Ref<Asset> AssetManager::get(const UUID_t& uuid) const
	{
		if (uuid.is_nil())
			return nullptr;

		auto it = m_Assets.find(uuid);
		if (it != m_Assets.end())
			return it->second;

		return nullptr;
	}

	Ref<Asset> AssetManager::get_by_name(const std::string& name) const
	{
		return this->get(this->get_uuid_by_name(name));
	}
} // namespace Na
