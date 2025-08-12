#include "Pch.hpp"
#include "Natrium/Assets/ImageAsset.hpp"

#include <stb/stb_image.h>

namespace Na {
	FileErrorCode ImageAsset::load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			return FileErrorCode::NotFound;
		}

		if (!std::filesystem::is_regular_file(path))
		{
			return FileErrorCode::InvalidFormat;
		}

		int channels;
		m_Data = stbi_load(
			path.string().c_str(),
			&m_Width,
			&m_Height,
			&channels,
			STBI_rgb_alpha
		);
		m_Size = (u64)m_Width * (u64)m_Height * 4;

		if (!m_Data)
		{
			//stbi_failure_reason();
			return FileErrorCode::Unknown;
		}

		return FileErrorCode::None;
	}
} // namespace Na
