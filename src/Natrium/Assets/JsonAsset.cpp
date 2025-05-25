#include "Pch.hpp"
#include "Natrium/Assets/JsonAsset.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	AssetHandle<JsonAsset> JsonAsset::Load(const std::filesystem::path& path)
	{
		std::ifstream file(path);
		NA_ASSERT(file, "Failed to Load JsonAsset: Error in loading {}", path.C_STR());

		AssetHandle<JsonAsset> asset = std::make_shared<JsonAsset>(nlohmann::json::parse(file));

		file.close();
	}
} // namespace Na