#include "Pch.hpp"
#include "Natrium/Assets/AssetManager.hpp"

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"
#include "Natrium/Assets/TextAsset.hpp"
#include "Natrium/Assets/RendererSettingsAsset.hpp"

namespace Na {
	AssetManager::AssetManager(
		const std::filesystem::path& engine_assets_directory,
		const std::filesystem::path& shader_output_directory
	)
	: m_EngineAssetsDirectory(engine_assets_directory),
	  m_ShaderOutputDirectory(shader_output_directory)
	{
		if (!std::filesystem::exists(m_EngineAssetsDirectory))
			std::filesystem::create_directories(m_EngineAssetsDirectory);

		if (!std::filesystem::exists(m_ShaderOutputDirectory))
			std::filesystem::create_directories(m_ShaderOutputDirectory);
	}

	void AssetManager::destroy(void)
	{
		m_ShaderOutputDirectory.clear();
		m_EngineAssetsDirectory.clear();
		m_Assets.clear();
	}

	void AssetManager::free_asset(const UUID_t& uuid)
	{
		auto it = m_Assets.find(uuid);
		if (it != m_Assets.end())
			m_Assets.erase(it);
	}

	Ref<Asset> AssetManager::get_asset(const UUID_t& uuid) const
	{
		auto it = m_Assets.find(uuid);
		if (it != m_Assets.end())
			return it->second;

		return nullptr;
	}

	auto AssetManager::load_shader(
		const std::filesystem::path& path_to_glsl,
		Graphics::ShaderStage stage,
		const std::string_view& entry_point
	) const -> Expected<UniqueRef<Graphics::Shader>, ShaderLoadingError>
	{
		TextAsset glsl;

		FileErrorCode file_err_code = glsl.load(path_to_glsl);
		if (file_err_code != FileErrorCode::None)
		{
			return Unexpected(ShaderLoadingError(file_err_code));
		}

		const std::filesystem::path& input_path = path_to_glsl;
		std::filesystem::path output_path = m_ShaderOutputDirectory / input_path.filename().replace_extension(".bin");

		g_Logger.print(Trace, "Creating shader: [");
		g_Logger.printf(Trace, "\tinput path: {}", input_path.string());
		g_Logger.printf(Trace, "\toutput path: {}", output_path.string());
		g_Logger.print(Trace, "]");

		bool should_compile = (
			!std::filesystem::exists(output_path) ||
			std::filesystem::last_write_time(input_path) > std::filesystem::last_write_time(output_path)
		);

		UniqueRef<Graphics::Shader> shader;

		switch (Graphics::Device::Get()->backend())
		{
			case Graphics::DeviceBackend::Vulkan:
			{
				if (!should_compile)
				{
					if (auto expected = VulkanImpl::LoadSpirV(output_path))
					{
						shader = MakeUnique<VulkanImpl::Shader>(
							expected.value(),
							stage,
							entry_point
						);
					} else
					{
						return Unexpected(ShaderLoadingError(expected.error()));
					}
				} else
				{
					if (auto expected = VulkanImpl::CompileToSpirV(
						glsl.str(),
						input_path.filename().string(),
						entry_point
					))
					{
						const ArrayList<u32>& spv = expected.value();

						std::ofstream output_file(output_path, std::ios::binary);
						if (!output_file)
						{
							return Unexpected(ShaderLoadingError(FileErrorCode::Unknown));
						}

						output_file.write((const char*)spv.ptr(), spv.size() * sizeof(u32));
						output_file.close();

						shader = MakeUnique<VulkanImpl::Shader>(spv, stage, entry_point);
					} else
					{
						return Unexpected(ShaderLoadingError(expected.error()));
					}
				}
			}
		}

		return shader;
	}

	auto AssetManager::load_renderer_settings(const std::string& path) -> Expected<Ref<RendererSettingsAsset>, FileErrorCode>
	{
		UUID_t uuid = UUID::Generate(path);
		if (auto it = m_Assets.find(uuid); it != m_Assets.end())
		{
			return dynamic_ref_cast<RendererSettingsAsset>(it->second);
		}

		auto asset = MakeRef<RendererSettingsAsset>(uuid);

		FileErrorCode err_code = asset->load(path);

		if (err_code != FileErrorCode::None)
		{
			g_Logger.printf(Info, "{} doesn't exist, creating using default values!", path);

			std::filesystem::path default_settings_path = m_EngineAssetsDirectory / "default_renderer_settings.json";

			TextAsset default_settings;
			default_settings.load(default_settings_path);

			std::ofstream settings_file(path, std::ios::binary);
			NA_VERIFY(settings_file, "Failed to open file {}", path);

			settings_file.write(default_settings.str().c_str(), default_settings.str().size());
			settings_file.close();

			err_code = asset->load(path);
			if (err_code != FileErrorCode::None)
			{
				return Unexpected(err_code);
			}
		}

		m_Assets[uuid] = asset;

		return asset;
	}
} // namespace Na
