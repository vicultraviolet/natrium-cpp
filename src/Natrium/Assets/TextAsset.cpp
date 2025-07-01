#include "Pch.hpp"
#include "Natrium/Assets/TextAsset.hpp"

namespace Na {
	void TextAsset::load(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::in | std::ios::binary);
		if (!file)
		{
			m_String.clear();
			return;
		}

		std::ostringstream ss;
		ss << file.rdbuf();
		m_String = ss.str();
	}
} // namespace Na
