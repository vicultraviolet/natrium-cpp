#include "Pch.hpp"
#include "Natrium/Assets/ImageAsset.hpp"

#include <stb/stb_image.h>

namespace Na {
	void ImageAsset::load(const std::filesystem::path& path)
	{
		int channels;
		m_Data = stbi_load(
			path.string().c_str(),
			&m_Width,
			&m_Height,
			&channels,
			STBI_rgb_alpha
		);
		m_Size = (u64)m_Width * (u64)m_Height * 4;
	}
} // namespace Na
