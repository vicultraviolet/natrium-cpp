#if !defined(NA_ASSETS_SERIALIZED_JSON_HPP)
#define NA_ASSETS_SERIALIZED_JSON_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	class SerializedJsonAsset : public Asset {
	public:
		SerializedJsonAsset(void) = default;
		SerializedJsonAsset(const UUID_t& uuid) : Asset(uuid) {}

		FileErrorCode load(const std::filesystem::path& path) override;
		FileErrorCode save(const std::filesystem::path& path) override;

		[[nodiscard]] virtual nlohmann::json serialize(void) const = 0;
		virtual void deserialize(const nlohmann::json& j) = 0;

		[[nodiscard]] virtual inline std::string to_string(void) const { return this->serialize().dump(4);  }
	};
} // namespace Na 

#endif // NA_ASSETS_SERIALIZED_JSON_HPP