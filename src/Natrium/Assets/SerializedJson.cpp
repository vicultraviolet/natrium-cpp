#include "Pch.hpp"
#include "Natrium/Assets/SerializedJson.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	FileErrorCode SerializedJsonAsset::load(const std::filesystem::path& path)
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
			g_Logger.printf(Error, "Failed to parse json file {}: {}", path.C_STR(), e.what());
			return FileErrorCode::InvalidFormat;
		}

		this->deserialize(json);

		return FileErrorCode::None;
	}

	FileErrorCode SerializedJsonAsset::save(const std::filesystem::path& path)
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
} // namespace Na 
