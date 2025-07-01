#if !defined(NA_ASSET_HPP)
#define NA_ASSET_HPP

#include "Natrium/Core/UUID.hpp"

namespace Na {
	class Asset {
	public:
		Asset(void) = default;
		Asset(const UUID_t& uuid) : m_UUID(uuid) {}

		virtual ~Asset(void) = default;

		virtual void load(const std::filesystem::path& path) {}

		[[nodiscard]] const UUID_t& uuid(void) const { return m_UUID; }
		[[nodiscard]] virtual operator bool(void) const = 0;
	private:
		UUID_t m_UUID;
	};
} // namespace Na

#endif // NA_ASSET_HPP