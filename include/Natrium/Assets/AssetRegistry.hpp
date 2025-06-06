#if !defined(NA_ASSET_REGISTRY_HPP)
#define NA_ASSET_REGISTRY_HPP

#include "Natrium/Assets/Asset.hpp"
#include "Natrium/Assets/ShaderAsset.hpp"
#include "Natrium/Assets/RendererSettingsAsset.hpp"
#include "Natrium/Graphics/ShaderModule.hpp"

namespace Na {
	class AssetRegistry {
	public:
		AssetRegistry(
			const std::filesystem::path& engine_asset_dir = "assets/engine/",
			const std::filesystem::path& shader_output_dir = "bin/shaders/"
		);
		inline ~AssetRegistry(void) { this->destroy(); }

		void destroy(void);

		inline void free_asset(const std::string& asset) { m_Assets.erase(asset); }
		inline void free_all(void) { m_Assets.clear(); }

		template<LoadableAsset T>
		inline AssetHandle<T> load_asset(const std::string& path)
		{
			auto it = m_Assets.find(path);
			if (it != m_Assets.end())
				return dynamic_pointer_cast<T>(it->second);

			AssetHandle<T> asset = T::Load(path);
			m_Assets[path] = asset;
			return asset;
		}

		inline AssetHandle<RendererSettings> load_asset(const std::string& path) { return load_renderer_settings(path); }

		AssetHandle<RendererSettings> load_renderer_settings(const std::string& path);

		ShaderModule create_shader_module_from_src(
			const std::string_view& src_path,
			ShaderStageBits stage,
			const std::string_view& entry_point = "main"
		) const;

		ShaderModule create_shader_module_from_str(
			const std::string_view& name,
			const std::string_view& src,
			ShaderStageBits stage,
			const std::string_view& entry_point = "main"
		) const;
	private:
		std::unordered_map<std::string, AssetHandle<>> m_Assets;
		std::filesystem::path m_EngineAssetDir;
		std::filesystem::path m_ShaderOutputDir;
	};
} // namespace Na

#endif // NA_ASSET_REGISTRY_HPP