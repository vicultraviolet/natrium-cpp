#if !defined(NA_UUID_HPP)
#define NA_UUID_HPP

#include "Natrium/Core.hpp"

namespace Na {
	using UUID_t = uuids::uuid;

	namespace UUID {
		[[nodiscard]] const UUID_t& Null(void);

		[[nodiscard]] UUID_t Generate(void);
		[[nodiscard]] UUID_t Generate(const std::string& name);
		[[nodiscard]] UUID_t Generate(const std::filesystem::path& path);

		[[nodiscard]] UUID_t FromString(const std::string& uuid_string);
		[[nodiscard]] std::string ToString(const UUID_t& uuid);

		[[nodiscard]] UUID_t FromBytes(const Byte* bytes);
		[[nodiscard]] UUID_t FromBytes(const std::array<Byte, 16>& bytes);

	} // namespace UUID
} // namespace Na

#endif // NA_UUID_HPP