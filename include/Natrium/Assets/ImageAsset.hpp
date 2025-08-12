#if !defined(NA_IMAGE_ASSET_HPP)
#define NA_IMAGE_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	// rgba8 image
	class ImageAsset : public Asset {
	public:
		ImageAsset(void) = default;
		ImageAsset(const UUID_t& uuid) : Asset(uuid) {}

		FileErrorCode load(const std::filesystem::path& path) override;

		~ImageAsset(void) { this->destroy(); }
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

#endif // NA_IMAGE_ASSET_HPP