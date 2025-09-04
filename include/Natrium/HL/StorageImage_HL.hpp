#if !defined(NA_HL_STORAGE_IMAGE_HPP)
#define NA_HL_STORAGE_IMAGE_HPP

#include "Natrium/Graphics/DeviceImage.hpp"
#include "Natrium/Graphics/Sampler.hpp"

#include "Natrium/Assets/HostImage.hpp"

#include "Natrium/Graphics/UniformSet.hpp"

namespace Na::HL {
	class StorageImage {
	public:
		StorageImage(void) = default;

		StorageImage(u32 width, u32 height);

		[[nodiscard]] inline View<Graphics::DeviceImage> img(void) { return m_Image; }
		[[nodiscard]] inline View<const Graphics::DeviceImage> img(void) const { return m_Image; }
	private:
		UniqueRef<Graphics::DeviceImage> m_Image;
	};
} // namespace Na::HL

#endif // NA_HL_STORAGE_IMAGE_HPP