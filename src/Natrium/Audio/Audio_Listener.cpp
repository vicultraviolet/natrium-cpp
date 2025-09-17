#include "Pch.hpp"
#include "Natrium/Audio/Audio_Listener.hpp"

namespace Na::Audio {
	float Listener::gain(void) const
	{
		float gain;

		alGetListenerf(AL_GAIN, &gain);

		return gain;
	}

	void Listener::set_gain(float gain)
	{
		alListenerf(AL_GAIN, gain);
	}

	glm::vec3 Listener::position(void) const
	{
		glm::vec3 position;

		alGetListenerfv(AL_POSITION, glm::value_ptr(position));

		return position;
	}

	void Listener::set_position(const glm::vec3& position)
	{
		alListenerfv(AL_POSITION, glm::value_ptr(position));
	}

	glm::vec3 Listener::velocity(void) const
	{
		glm::vec3 velocity;

		alGetListenerfv(AL_VELOCITY, glm::value_ptr(velocity));

		return velocity;
	}

	void Listener::set_velocity(const glm::vec3& velocity)
	{
		alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
	}

	ListenerOrientation Listener::orientation(void) const
	{
		ListenerOrientation orientation{};

		alGetListenerfv(AL_ORIENTATION, (float*)&orientation);

		return orientation;
	}

	void Listener::set_orientation(const ListenerOrientation& orientation)
	{
		alListenerfv(AL_ORIENTATION, (float*)&orientation);
	}
} // namespace Na::Audio
