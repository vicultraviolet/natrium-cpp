#include "Pch.hpp"
#include "Natrium/Graphics/RendererSettings.hpp"

#include "Natrium/Graphics/Device.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

#include "Natrium/Assets/AssetManager.hpp"

namespace Na {
	FileErrorCode RendererSettings::_handle_missing(const std::filesystem::path& path)
	{
		g_Logger.printf(Info, "{} doesn't exist, creating using default values!", path.C_STR());

		auto default_settings_path = AssetManager::Get()->engine_assets_dir() / "default_renderer_settings.json";

		FileErrorCode code = FileErrorCode::None;
		
		code = this->load(default_settings_path);
		if (code != FileErrorCode::None)
			return code;

		code = this->save(path);
		if (code != FileErrorCode::None)
			return code;

		return FileErrorCode::None;
	}		

	void RendererSettings::set_max_anisotropy(float anisotropy)
	{
		this->max_anisotropy = std::min(
			anisotropy,
			Graphics::Device::Get()->limits()->max_anisotropy()
		);
	}

	[[nodiscard]] nlohmann::json RendererSettings::serialize(void) const
	{
		nlohmann::json j;

		j["max_frames_in_flight"]  = this->max_frames_in_flight;
		j["anisotropy_enabled"]    = this->anisotropy_enabled;
		j["max_anisotropy"]        = this->max_anisotropy;
		j["multisampling_enabled"] = this->multisampling_enabled;

		return j;
	}

	void RendererSettings::deserialize(const nlohmann::json& j)
	{
		this->max_frames_in_flight  = j["max_frames_in_flight"];
		this->anisotropy_enabled	= j["anisotropy_enabled"];
		this->max_anisotropy		= j["max_anisotropy"];
		this->multisampling_enabled	= j["multisampling_enabled"];
	}
} // namespace Na