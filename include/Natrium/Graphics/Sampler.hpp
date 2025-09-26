#if !defined(NA_GRAPHICS_SAMPLER_HPP)
#define NA_GRAPHICS_SAMPLER_HPP

#include "Natrium/Graphics/DeviceImage.hpp"
#include "Natrium/Graphics/RendererSettings.hpp"

namespace Na::Graphics {
	enum class SamplerFilter : u8 {
		None = 0,
		Nearest,
		Linear
	};

	enum class SamplerAddressMode : u8 {
		None = 0,
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge,
	};

	struct SamplerCreateInfo {
		SamplerFilter oversampling_filter;
		SamplerFilter undersampling_filter;

		SamplerAddressMode adress_mode_u = SamplerAddressMode::Repeat;
		SamplerAddressMode adress_mode_v = SamplerAddressMode::Repeat;

		WeakRef<const RendererSettings> renderer_settings;
	};
	
	class Sampler {
	public:
		[[nodiscard]] static UniqueRef<Sampler> Make(const SamplerCreateInfo& info);
		virtual ~Sampler(void) = default;

		[[nodiscard]] virtual operator bool(void) const = 0;
	};
} // namespace Na::Graphics

#endif // NA_GRAPHICS_SAMPLER_HPP