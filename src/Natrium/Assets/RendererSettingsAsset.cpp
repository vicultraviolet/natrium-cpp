#include "Pch.hpp"
#include "Natrium/Assets/RendererSettingsAsset.hpp"

#include "Natrium/Graphics/Device.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	FileErrorCode RendererSettingsAsset::load(const std::filesystem::path& path)
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

		m_Path = path;

		try
		{
			m_Json = nlohmann::json::parse(file);
		} catch (const nlohmann::json::parse_error& e)
		{
			g_Logger.printf(Error, "Failed to parse renderer settings file {}: {}", path.C_STR(), e.what());
			return FileErrorCode::InvalidFormat;
		}

		return FileErrorCode::None;
	}

	void RendererSettingsAsset::set_all(const RendererSettingsAsset& other)
	{
		m_Json = other.m_Json;
		this->_update_file();
	}

	void RendererSettingsAsset::set_max_frames_in_flight(u32 max_frames_in_flight)
	{
		m_Json["max_frames_in_flight"] = max_frames_in_flight;
		this->_update_file();
	}

	void RendererSettingsAsset::set_anisotropy_enabled(bool enabled)
	{
		m_Json["anisotropy_enabled"] = enabled;
		this->_update_file();
	}

	void RendererSettingsAsset::set_max_anisotropy(float max_anisotropy)
	{
		m_Json["max_anisotropy"] = std::min(max_anisotropy, Graphics::Device::Get()->limits()->max_anisotropy());
		this->_update_file();
	}

	void RendererSettingsAsset::set_multisampling_enabled(bool enabled)
	{
		m_Json["multisampling_enabled"] = enabled;
		this->_update_file();
	}

	u32 RendererSettingsAsset::max_frames_in_flight(void) const
	{
		return m_Json["max_frames_in_flight"];
	}

	bool RendererSettingsAsset::anisotropy_enabled(void) const
	{
		return m_Json["anisotropy_enabled"];
	}

	float RendererSettingsAsset::max_anisotropy(void) const
	{
		return m_Json["max_anisotropy"];
	}

	bool RendererSettingsAsset::multisampling_enabled(void) const
	{
		return m_Json["multisampling_enabled"];
	}

	bool RendererSettingsAsset::set_json_with_str(std::string_view json_str)
	{
		if (!nlohmann::json::accept(json_str))
		{
			return false;
		}

		m_Json = nlohmann::json::parse(json_str);
		this->_update_file();

		return true;
	}

	std::string RendererSettingsAsset::json_as_str(void) const
	{
		return m_Json.dump(4);
	}

	void RendererSettingsAsset::_update_file(void) const
	{
		if (m_Path.empty())
			return;

		std::ofstream file(m_Path, std::ios::trunc);
		NA_ASSERT(file, "Failed to update renderer settings file: Error in loading {}!", m_Path.C_STR());

		file << m_Json.dump(4);
		file.close();
	}
} // namespace Na