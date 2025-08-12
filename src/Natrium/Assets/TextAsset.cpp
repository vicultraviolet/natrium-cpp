#include "Pch.hpp"
#include "Natrium/Assets/TextAsset.hpp"

namespace Na {
	FileErrorCode TextAsset::load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			return FileErrorCode::NotFound;
		}

		if (!std::filesystem::is_regular_file(path))
		{
			return FileErrorCode::InvalidFormat;
		}

		std::ifstream file(path, std::ios::in | std::ios::binary);
		if (!file)
		{
			return FileErrorCode::Unknown;
		}

		std::ostringstream ss;
		ss << file.rdbuf();
		m_String = ss.str();

		return FileErrorCode::None;
	}
} // namespace Na
