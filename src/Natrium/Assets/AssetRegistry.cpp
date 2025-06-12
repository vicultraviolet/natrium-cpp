#include "Pch.hpp"
#include "Natrium/Assets/AssetRegistry.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	extern ArrayList<Byte> LoadSpv(const std::filesystem::path& path);

	AssetRegistry::AssetRegistry(
		const std::filesystem::path& engine_asset_dir,
		const std::filesystem::path& shader_output_dir
	)
	: m_EngineAssetDir(engine_asset_dir),
	m_ShaderOutputDir(shader_output_dir)
	{
		std::filesystem::create_directories(m_EngineAssetDir);
		std::filesystem::create_directories(m_ShaderOutputDir);
	}

	void AssetRegistry::destroy(void)
	{
		m_ShaderOutputDir.clear();
		m_EngineAssetDir.clear();
		m_Assets.clear();
	}

	AssetHandle<RendererSettings> AssetRegistry::load_renderer_settings(const std::string& path)
	{
		auto it = m_Assets.find(path);
		if (it != m_Assets.end())
			return dynamic_pointer_cast<RendererSettings>(it->second);

		if (!std::filesystem::exists(path))
		{
			g_Logger.printf(Info, "{} doesn't exist, creating using default values!", path);

			std::filesystem::path default_settings_path = m_EngineAssetDir / "default_renderer_settings.json";

			std::ifstream default_settings_file(
				default_settings_path,
				std::ios::ate | std::ios::binary
			);
			NA_ASSERT(default_settings_file, "Failed to open file {}", default_settings_path.string());

			u64 size = default_settings_file.tellg();
			std::string default_settings_str(size, ' ');

			default_settings_file.seekg(0);
			default_settings_file.read(default_settings_str.data(), size);
			default_settings_file.close();

			std::ofstream settings_file(path, std::ios::binary);
			NA_VERIFY(settings_file, "Failed to open file {}", path);

			settings_file.write(default_settings_str.c_str(), size);
			settings_file.close();
		}

		AssetHandle<RendererSettings> asset = RendererSettings::Load(path);
		m_Assets[path] = asset;
		return asset;
	}

	ShaderModule AssetRegistry::create_shader_module_from_src(
		const std::string_view& src_path,
		ShaderStageBits stage,
		const std::string_view& entry_point
	) const
	{
		std::filesystem::path input_path = src_path;
		std::filesystem::path output_path = m_ShaderOutputDir / input_path.filename().replace_extension(".spv");

		g_Logger.print (Trace, "Creating shader module: [");
		g_Logger.printf(Trace, "\tinput path: {}", src_path);
		g_Logger.printf(Trace, "\toutput path: {}", output_path.C_STR());
		g_Logger.print (Trace, "]");

		if (std::filesystem::exists(output_path) &&
			std::filesystem::last_write_time(output_path)
				> std::filesystem::last_write_time(input_path))
			return ShaderModule(LoadSpv(output_path), stage, entry_point);

		ShaderBinary shader_binary(ShaderString(src_path).compile());

		std::ofstream output_file(output_path, std::ios::binary);
		NA_ASSERT(output_file, "Failed to open file {}", output_path.C_STR());

		output_file.write((const char*)shader_binary.ptr(), shader_binary.size());
		output_file.close();

		return ShaderModule(shader_binary, stage, entry_point);
	}

	ShaderModule AssetRegistry::create_shader_module_from_str(
		const std::string_view& name,
		const std::string_view& src,
		ShaderStageBits stage,
		const std::string_view& entry_point
	) const
	{
		std::filesystem::path output_path = (m_ShaderOutputDir / name).replace_extension(".spv");

		ShaderBinary shader_binary(ShaderString(src, name).compile());

		std::ofstream output_file(output_path, std::ios::binary);
		NA_ASSERT(output_file, "Failed to open file {}", output_path.C_STR());

		output_file.write((const char*)shader_binary.ptr(), shader_binary.size());
		output_file.close();

		return ShaderModule(shader_binary, stage, entry_point);
	}
} // namespace Na
