#if !defined(NA_AUDIO_SOURCE_HPP)
#define NA_AUDIO_SOURCE_HPP

#include "Natrium/Audio/Audio_Buffer.hpp"

namespace Na::Audio {
	class Source {
	public:
		Source(void) = default;

		~Source(void) { this->destroy(); }
		void destroy(void);

		explicit Source(const Audio::Buffer& buffer);

		Source(Source&& other) noexcept;
		Source& operator=(Source&& other) noexcept;

		void play(void) const;
		void stop(void) const;

		[[nodiscard]] float gain(void) const;
		void set_gain(float gain);

		[[nodiscard]] glm::vec3 position(void) const;
		void set_position(const glm::vec3& position);

		[[nodiscard]] inline ALuint native(void) const { return m_Source; }

		[[nodiscard]] inline operator bool(void) const { return m_Source != u32max; }
	private:
		ALuint m_Source = u32max;
	};
} // namespace Na::Audio

#endif // NA_AUDIO_SOURCE_HPP