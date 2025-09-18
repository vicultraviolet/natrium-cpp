#if !defined(NA_AUDIO_SOURCE_HPP)
#define NA_AUDIO_SOURCE_HPP

#include "Natrium/Audio/Audio_Buffer.hpp"

namespace Na::Audio {
	struct SourceCreateInfo {

	};

	class Source {
	public:
		Source(void) = default;
		Source(const SourceCreateInfo& info);

		~Source(void) { this->destroy(); }
		void destroy(void);

		Source(const Source& other) = default;
		Source& operator=(const Source& other) = default;

		Source(Source&& other) noexcept;
		Source& operator=(Source&& other) noexcept;

		[[nodiscard]] inline Audio::Buffer attached_buffer(void) const { return m_Buffer; }
		void attach_buffer(Audio::Buffer);

		void play(void) const;
		void stop(void) const;

		[[nodiscard]] glm::vec3 position(void) const;
		void set_position(const glm::vec3&);

		[[nodiscard]] glm::vec3 velocity(void) const;
		void set_velocity(const glm::vec3&);

		[[nodiscard]] glm::vec3 direction(void) const;
		void set_direction(const glm::vec3&);

		[[nodiscard]] float pitch(void) const;
		void set_pitch(float);

		[[nodiscard]] float gain(void) const;
		void set_gain(float);

		[[nodiscard]] float min_gain(void) const;
		void set_min_gain(float);

		[[nodiscard]] float max_gain(void) const;
		void set_max_gain(float);

		[[nodiscard]] bool looping_enabled(void) const;
		void set_looping_enabled(bool);

		[[nodiscard]] float max_distance(void) const;
		void set_max_distance(float);

		// default is 1
		[[nodiscard]] float rolloff_rate(void) const;

		// default is 1
		void set_rolloff_rate(float);

		[[nodiscard]] std::chrono::milliseconds offset(void) const;
		void set_offset(std::chrono::milliseconds);

		[[nodiscard]] float cone_outer_gain(void) const;
		void set_cone_outer_gain(float);

		[[nodiscard]] float cone_inner_gain(void) const;
		void set_cone_inner_gain(float);

		// default is 360
		[[nodiscard]] float cone_angle(void) const;

		// default is 360
		void set_cone_angle(float);

		[[nodiscard]] inline ALuint native(void) const { return m_Source; }

		[[nodiscard]] inline operator bool(void) const { return m_Source != u32max; }
		[[nodiscard]] inline bool operator==(Source source) { return m_Source == source.m_Source; }
	private:
		ALuint m_Source = u32max;
		Audio::Buffer m_Buffer;
	};
} // namespace Na::Audio

#endif // NA_AUDIO_SOURCE_HPP