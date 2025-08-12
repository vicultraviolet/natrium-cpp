#if !defined(NA_TEXT_ASSET_HPP)
#define NA_TEXT_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	class TextAsset : public Asset {
	public:
		TextAsset(void) = default;
		TextAsset(const UUID_t& uuid) : Asset(uuid) {}

		FileErrorCode load(const std::filesystem::path& path) override;

		[[nodiscard]] inline std::string& str(void) { return m_String; }
		[[nodiscard]] inline const std::string& str(void) const { return m_String; }

		[[nodiscard]] inline operator bool(void) const override { return !m_String.empty(); }
	private:
		std::string m_String;
	};
} // namespace Na

#endif // NA_TEXT_ASSET_HPP