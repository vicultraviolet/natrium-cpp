#if !defined(NA_AUDIO_WAV_HPP)
#define NA_AUDIO_WAV_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na::Audio {
    class Wav : public Asset {
    public:
		Wav(void) = default;
		~Wav(void) = default;

		Wav(const UUID_t& uuid) : Asset(uuid) {}

		FileErrorCode load(const std::filesystem::path& path) override;

		[[nodiscard]] inline u32 channel_count(void) const { return m_ChannelCount; }

		[[nodiscard]] inline u32 sample_rate(void) const { return m_SampleRate; }
		[[nodiscard]] inline u32 bit_depth(void) const { return m_BitDepth; }

		[[nodiscard]] inline u64 sample_count(void) const { return m_SampleCount; }

		[[nodiscard]] inline float duration(void) const { return m_Duration; }

		[[nodiscard]] inline bool is_mono(void) const { return m_ChannelCount == 1; }
		[[nodiscard]] inline bool is_stereo(void) const { return m_ChannelCount == 2; }

		[[nodiscard]] inline ArrayList<Byte>& data(void) { return m_Data; }
		[[nodiscard]] inline const ArrayList<Byte>& data(void) const { return m_Data; }

		[[nodiscard]] inline operator bool(void) const { return m_ChannelCount != 0; }
    private:
		u32 m_ChannelCount = 0;

		u32 m_SampleRate = 0;
		u32 m_BitDepth = 0;

		u64 m_SampleCount = 0;

		float m_Duration = 0.0f;

		ArrayList<Byte> m_Data;
    };

} // namespace Na::Audio

#endif // NA_AUDIO_WAV_HPP