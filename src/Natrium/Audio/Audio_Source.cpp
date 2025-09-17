#include "Pch.hpp"
#include "Natrium/Audio/Audio_Source.hpp"

namespace Na::Audio {
	Source::Source(const Audio::Buffer& buffer)
	{
		alGenSources(1, &m_Source);

		alSourcei(m_Source, AL_BUFFER, buffer.native());
	}

	void Source::destroy(void)
	{
		if (m_Source != u32max)
		{
			alDeleteSources(1, &m_Source);
			m_Source = u32max;
		}
	}

	void Source::play(void) const
	{
		alSourcePlay(m_Source);
	}

	void Source::stop(void) const
	{
		alSourceStop(m_Source);
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
