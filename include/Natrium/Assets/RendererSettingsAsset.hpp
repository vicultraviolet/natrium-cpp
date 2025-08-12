#if !defined(NA_RENDERER_SETTINGS_ASSET_HPP)
#define NA_RENDERER_SETTINGS_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	class RendererSettingsAsset : public Asset {
	public:
		RendererSettingsAsset(void) = default;
		RendererSettingsAsset(const UUID_t& uuid) : Asset(uuid) {}

		~RendererSettingsAsset(void) = default;

		FileErrorCode load(const std::filesystem::path& path) override;

		void set_all(const RendererSettingsAsset& other);

		void set_max_frames_in_flight(u32 max_frames_in_flight);
		void set_anisotropy_enabled(bool enabled);
		void set_max_anisotropy(float max_anisotropy);
		void set_multisampling_enabled(bool enabled);

		[[nodiscard]] u32 max_frames_in_flight(void) const;
		[[nodiscard]] bool anisotropy_enabled(void) const;
		[[nodiscard]] float max_anisotropy(void) const;
		[[nodiscard]] bool multisampling_enabled(void) const;

		bool set_json_with_str(std::string_view json_str);
		[[nodiscard]] std::string json_as_str(void) const;

		[[nodiscard]] inline operator bool(void) const override { return m_Json; }
	private:
		void _update_file(void) const;
	private:
		nlohmann::json m_Json;
		std::filesystem::path m_Path;
	};
} // namespace Na

#endif // NA_RENDERER_SETTINGS_ASSET_HPP