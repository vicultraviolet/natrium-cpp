#include "Pch.hpp"
#include "Natrium/Graphics/RendererSettings.hpp"

#include "Natrium/Graphics/Device.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	void RendererSettings::set_max_anisotropy(float anisotropy)
	{
		this->max_anisotropy = std::min(
			anisotropy,
			Graphics::Device::Get()->limits()->max_anisotropy()
		);
	}

	[[nodiscard]] nlohmann::json RendererSettings::serialize(void) const
	{
		nlohmann::json j;

		j["max_frames_in_flight"]  = this->max_frames_in_flight;
		j["anisotropy_enabled"]    = this->anisotropy_enabled;
		j["max_anisotropy"]        = this->max_anisotropy;
		j["multisampling_enabled"] = this->multisampling_enabled;

		return j;
	}

	void RendererSettings::deserialize(const nlohmann::json& j)
	{
		this->max_frames_in_flight  = j["max_frames_in_flight"];
		this->anisotropy_enabled	= j["anisotropy_enabled"];
		this->max_anisotropy		= j["max_anisotropy"];
		this->multisampling_enabled	= j["multisampling_enabled"];
	}
} // namespace Na