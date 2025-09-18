#if !defined(NA_AUDIO_LISTENER_HPP)
#define NA_AUDIO_LISTENER_HPP

#include "Natrium/Core.hpp"

namespace Na::Audio {
	struct ListenerOrientation {
		glm::vec3 forward, up;
	};

	class Listener {
	public:
		[[nodiscard]] float gain(void) const;
		void set_gain(float gain);

		[[nodiscard]] glm::vec3 position(void) const;
		void set_position(const glm::vec3& position);

		[[nodiscard]] glm::vec3 velocity(void) const;
		void set_velocity(const glm::vec3& velocity);

		[[nodiscard]] ListenerOrientation orientation(void) const;
		void set_orientation(const ListenerOrientation& orientation);
	};
} // namespace Na::Audio

#endif // NA_AUDIO_LISTENER_HPP