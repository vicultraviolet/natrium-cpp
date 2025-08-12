#if !defined(NA_ASSET_MANAGER_HPP)
#define NA_ASSET_MANAGER_HPP

#include "Natrium/Assets/Asset.hpp"
#include "Natrium/Graphics/Shader.hpp"

#include "Natrium/Assets/RendererSettingsAsset.hpp"

#include "Natrium/Core/ErrorCodes.hpp"

namespace Na {
	struct ShaderLoadingError {
		FileErrorCode file_err_code = FileErrorCode::None;
		ShaderErrorCode shader_err_code = ShaderErrorCode::None;

		ShaderLoadingError(FileErrorCode file_err_code) : file_err_code(file_err_code) {}
		ShaderLoadingError(ShaderErrorCode shader_err_code) : shader_err_code(shader_err_code) {}
		ShaderLoadingError(FileErrorCode file_err_code, ShaderErrorCode shader_err_code)
		: file_err_code(file_err_code), shader_err_code(shader_err_code)
		{}
	};

	class AssetManager {
	public:
		AssetManager(void) = default;
		AssetManager(
			const std::filesystem::path& engine_assets_directory,
			const std::filesystem::path& shader_output_directory
		);

		~AssetManager(void) { this->destroy(); }
		void destroy(void);

		[[nodiscard]] auto load_shader(
			const std::filesystem::path& path_to_glsl,
			Graphics::ShaderStage stage,
			const std::string_view& entry_point = "main"
		) const -> Expected<UniqueRef<Graphics::Shader>, ShaderLoadingError>;

		template<typename T>
		[[nodiscard]] auto load_asset(const std::string& path) -> Expected<Ref<T>, FileErrorCode>
		{
			static_assert(std::is_base_of<Asset, T>::value, "Failed to load asset: T must inherit from FileAsset!");

			UUID_t uuid = UUID::Generate(path);
			if (auto it = m_Assets.find(uuid); it != m_Assets.end())
			{
				return dynamic_ref_cast<T>(it->second);
			}

			Ref<T> asset = Ref<T>::Make(uuid);

			FileErrorCode err_code = asset->load(path);
			if (err_code != FileErrorCode::None)
			{
				return Unexpected(err_code);
			}

			m_Assets[uuid] = asset;

			return asset;
		}

		[[nodiscard]] auto load_asset(const std::string& path) -> Expected<Ref<RendererSettingsAsset>, FileErrorCode>;

		template<typename T>
		[[nodiscard]] inline Expected<Ref<T>, FileErrorCode> load_asset_p(const std::filesystem::path& path) { return this->load_asset<T>(path.string()); }

		void free_asset(const UUID_t& uuid);

		[[nodiscard]] Ref<Asset> get_asset(const UUID_t& uuid) const;

		template<typename T>
		[[nodiscard]] Ref<T> get_asset(const UUID_t& uuid) const
		{
			static_assert(std::is_base_of<Asset, T>::value, "Failed to get asset: T must inherit from Asset!");
			return dynamic_ref_cast<T>(this->get_asset(uuid));
		}
	private:
		std::unordered_map<UUID_t, Ref<Asset>> m_Assets;
		std::filesystem::path m_EngineAssetsDirectory = "engine/assets/";
		std::filesystem::path m_ShaderOutputDirectory = "bin/shaders/";
	};
} // namespace Na

#endif // NA_ASSET_MANAGER_HPP