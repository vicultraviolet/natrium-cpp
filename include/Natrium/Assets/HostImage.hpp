#if !defined(NA_ASSETS_HOST_IMAGE_HPP)
#define NA_ASSETS_HOST_IMAGE_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	// rgba8 image
	class HostImage : public Asset {
	public:
		HostImage(void) = default;
		HostImage(const UUID_t& uuid) : Asset(uuid) {}

		FileErrorCode load(const std::filesystem::path& path) override;
		FileErrorCode save(const std::filesystem::path& path) override;

		~HostImage(void) { this->destroy(); }
		inline void destroy(void) { free(m_Data); m_Data = nullptr; }

		[[nodiscard]] inline void* data(void) const { return m_Data; }
		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline int width(void) const { return m_Width; }
		[[nodiscard]] inline int height(void) const { return m_Height; }

		[[nodiscard]] inline operator bool(void) const override { return m_Data; }
	private:
		void* m_Data = nullptr;
		u64 m_Size = 0;
		int m_Width = -1, m_Height = -1;
	};
} // namespace Na

#endif // NA_ASSETS_HOST_IMAGE_HPP