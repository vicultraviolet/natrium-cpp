#if !defined(NA_JSON_ASSET_HPP)
#define NA_JSON_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	class JsonAsset : public Asset {
	public:
		JsonAsset(const nlohmann::json& json) : m_Json(json) {}
		~JsonAsset(void) override = default;

		static AssetHandle<JsonAsset> Load(const std::filesystem::path& path);

		[[nodiscard]] inline nlohmann::json& json(void) { return m_Json; }
		[[nodiscard]] inline const nlohmann::json& json(void) const { return m_Json; }

		[[nodiscard]] inline nlohmann::json& get(void) { return m_Json; }
		[[nodiscard]] inline const nlohmann::json& get(void) const { return m_Json; }

		[[nodiscard]] inline operator bool(void) const override { return m_Json; };
	private:
		nlohmann::json m_Json;
	};
	using Json = JsonAsset;
} // namespace Na

#endif // NA_JSON_ASSET_HPP