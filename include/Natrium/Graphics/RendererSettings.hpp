#if !defined(NA_GRAPHICS_RENDERER_SETTINGS_HPP)
#define NA_GRAPHICS_RENDERER_SETTINGS_HPP

#include "Natrium/Assets/SerializedJson.hpp"

namespace Na {
	class RendererSettings : public SerializedJsonAsset {
	public:
		u32 max_frames_in_flight = 0;

		bool anisotropy_enabled = false;
		float max_anisotropy = 0.0f;

		bool multisampling_enabled = false;

		RendererSettings(void) = default;
		RendererSettings(const UUID_t& uuid) : SerializedJsonAsset(uuid) {}

		~RendererSettings(void) = default;

		void set_max_anisotropy(float anisotropy);

		[[nodiscard]] nlohmann::json serialize(void) const override;
		void deserialize(const nlohmann::json& j) override;

		[[nodiscard]] inline operator bool(void) const override { return max_frames_in_flight; }
	protected:
		FileErrorCode _handle_missing(const std::filesystem::path& path) override;
	};
} // namespace Na

#endif // NA_GRAPHICS_RENDERER_SETTINGS_HPP