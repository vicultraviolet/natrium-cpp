#include "Pch.hpp"
#include "Natrium/Assets/ImageAsset.hpp"

#include <stb/stb_image.h>

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	AssetHandle<ImageAsset> ImageAsset::Load(const std::filesystem::path& path)
	{
		AssetHandle<ImageAsset> img_asset = Ref<ImageAsset>::Make();

		int channels;
		img_asset->m_Data = (void*)stbi_load(
			path.C_STR(),
			&img_asset->m_Width,
			&img_asset->m_Height,
			&channels,
			STBI_rgb_alpha
		);
		img_asset->m_Size = (u64)img_asset->m_Width * img_asset->m_Height * 4;

		return img_asset;
	}
} // namespace Na
