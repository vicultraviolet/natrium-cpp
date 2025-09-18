#include "Pch.hpp"
#include "Natrium/Audio/Audio_Source.hpp"

namespace Na::Audio {
	Source::Source(const SourceCreateInfo& info)
	{
		alGenSources(1, &m_Source);
	}

	void Source::destroy(void)
	{
		if (m_Source != u32max)
		{
			alDeleteSources(1, &m_Source);
			m_Source = u32max;
		}
	}

	void Source::attach_buffer(Audio::Buffer buffer)
	{
		m_Buffer = std::move(buffer);
		alSourcei(m_Source, AL_BUFFER, m_Buffer.native());
	}

	void Source::play(void) const
	{
		alSourcePlay(m_Source);
	}

	void Source::stop(void) const
	{
		alSourceStop(m_Source);
	}

	glm::vec3 Source::position(void) const
	{
		glm::vec3 position;
		alGetSourcefv(m_Source, AL_POSITION, glm::value_ptr(position));
		return position;
	}

	void Source::set_position(const glm::vec3& position)
	{
		alSourcefv(m_Source, AL_POSITION, glm::value_ptr(position));
	}

	glm::vec3 Source::velocity(void) const
	{
		glm::vec3 velocity;
		alGetSourcefv(m_Source, AL_VELOCITY, glm::value_ptr(velocity));
		return velocity;
	}

	void Source::set_velocity(const glm::vec3& velocity)
	{
		alSourcefv(m_Source, AL_VELOCITY, glm::value_ptr(velocity));
	}

	glm::vec3 Source::direction(void) const
	{
		glm::vec3 direction;
		alGetSourcefv(m_Source, AL_DIRECTION, glm::value_ptr(direction));
		return direction;
	}

	void Source::set_direction(const glm::vec3& direction)
	{
		alSourcefv(m_Source, AL_DIRECTION, glm::value_ptr(direction));
	}

	float Source::pitch(void) const
	{
		float pitch;
		alGetSourcef(m_Source, AL_PITCH, &pitch);
		return pitch;
	}

	void Source::set_pitch(float pitch)
	{
		alSourcef(m_Source, AL_PITCH, pitch);
	}

	float Source::gain(void) const
	{
		float gain;
		alGetSourcef(m_Source, AL_GAIN, &gain);
		return gain;
	}

	void Source::set_gain(float gain)
	{
		alSourcef(m_Source, AL_GAIN, gain);
	}

	float Source::max_gain(void) const
	{
		float gain;
		alGetSourcef(m_Source, AL_MAX_GAIN, &gain);
		return gain;
	}

	void Source::set_max_gain(float gain)
	{
		alSourcef(m_Source, AL_MAX_GAIN, gain);
	}

	float Source::min_gain(void) const
	{
		float gain;
		alGetSourcef(m_Source, AL_MIN_GAIN, &gain);
		return gain;
	}

	void Source::set_min_gain(float gain)
	{
		alSourcef(m_Source, AL_MIN_GAIN, gain);
	}

	bool Source::looping_enabled(void) const
	{
		int looping;
		alGetSourcei(m_Source, AL_LOOPING, &looping);
		return looping;
	}

	void Source::set_looping_enabled(bool looping)
	{
		alSourcei(m_Source, AL_LOOPING, looping);
	}

	float Source::max_distance(void) const
	{
		float distance;
		alGetSourcef(m_Source, AL_MAX_DISTANCE, &distance);
		return distance;
	}

	void Source::set_max_distance(float distance)
	{
		alSourcef(m_Source, AL_MAX_DISTANCE, distance);
	}

	float Source::rolloff_rate(void) const
	{
		float rate;
		alGetSourcef(m_Source, AL_ROLLOFF_FACTOR, &rate);
		return rate;
	}

	void Source::set_rolloff_rate(float rate)
	{
		alSourcef(m_Source, AL_ROLLOFF_FACTOR, rate);
	}

	std::chrono::milliseconds Source::offset(void) const
	{
		float offset;
		alGetSourcef(m_Source, AL_SEC_OFFSET, &offset);

		return std::chrono_literals::operator""ms((u64)(offset * 1000));
	}

	void Source::set_offset(std::chrono::milliseconds offset)
	{
		alSourcef(m_Source, AL_SEC_OFFSET, (float)offset.count() / 1000.0f);
	}

	float Source::cone_outer_gain(void) const
	{
		float gain;
		alGetSourcef(m_Source, AL_CONE_OUTER_GAIN, &gain);
		return gain;
	}

	void Source::set_cone_outer_gain(float gain)
	{
		alSourcef(m_Source, AL_CONE_OUTER_GAIN, gain);
	}

	float Source::cone_inner_gain(void) const
	{
		float gain;
		alGetSourcef(m_Source, AL_CONE_INNER_ANGLE, &gain);
		return gain;
	}

	void Source::set_cone_inner_gain(float gain)
	{
		alSourcef(m_Source, AL_CONE_INNER_ANGLE, gain);
	}

	float Source::cone_angle(void) const
	{
		float angle;
		alGetSourcef(m_Source, AL_CONE_OUTER_ANGLE, &angle);
		return angle;
	}

	void Source::set_cone_angle(float angle)
	{
		alSourcef(m_Source, AL_CONE_OUTER_ANGLE, angle);
	}

	Source::Source(Source&& other) noexcept
	: m_Source(std::exchange(other.m_Source, u32max))
	{

	}

	Source& Source::operator=(Source&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->destroy();

		m_Source = std::exchange(other.m_Source, u32max);

		return *this;
	}
} // namespace Na::Audio
