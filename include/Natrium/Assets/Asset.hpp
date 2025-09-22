#if !defined(NA_ASSET_HPP)
#define NA_ASSET_HPP

#include "Natrium/Core/UUID.hpp"
#include "Natrium/Core/ErrorCodes.hpp"

namespace Na {
	class Asset {
	public:
		Asset(void) = default;
		Asset(const UUID_t& uuid) : m_UUID(uuid) {}

		virtual ~Asset(void) = default;

		virtual inline FileErrorCode load(const std::filesystem::path& path)
		{
			if (!std::filesystem::exists(path))
			{
				return this->_handle_missing(path);
			}

			if (!std::filesystem::is_regular_file(path))
			{
				return FileErrorCode::InvalidFormat;
			}

			return FileErrorCode::None;
		}

		virtual inline FileErrorCode save(const std::filesystem::path& path)
		{
			if (std::filesystem::exists(path) &&
				!std::filesystem::is_regular_file(path))
			{
				return FileErrorCode::InvalidFormat;
			}

			return FileErrorCode::None;
		}

		[[nodiscard]] const UUID_t& uuid(void) const { return m_UUID; }
		[[nodiscard]] virtual operator bool(void) const = 0;

	protected:
		virtual inline FileErrorCode _handle_missing(const std::filesystem::path& path)
		{
			return FileErrorCode::NotFound;
		}

	protected:
		UUID_t m_UUID;
	};
} // namespace Na

#endif // NA_ASSET_HPP