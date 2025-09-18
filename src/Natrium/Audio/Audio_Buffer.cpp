#include "Pch.hpp"
#include "Natrium/Audio/Audio_Buffer.hpp"

namespace Na::Audio {
	static ALenum getWavFormat(const Wav& wav)
	{
		if (wav.is_mono())
		{
			switch (wav.bit_depth())
			{
				case 8:  return AL_FORMAT_MONO8;
				case 16: return AL_FORMAT_MONO16;
			}
		} else
		if (wav.is_stereo())
		{
			switch (wav.bit_depth())
			{
				case 8:  return AL_FORMAT_STEREO8;
				case 16: return AL_FORMAT_STEREO16;
			}
		}

		return 0;
	}

	Buffer::Buffer(WeakRef<Wav> _wav)
	{
		auto wav = _wav.lock();
		if (!wav)
			return;

		alGenBuffers(1, &m_Buffer);

		alBufferData(
			m_Buffer,
			getWavFormat(*wav),
			wav->data().ptr(),
			(ALsizei)wav->data().size(),
			wav->sample_rate()
		);
	}

	void Buffer::destroy(void)
	{
		if (m_Buffer != u32max)
		{
			alDeleteBuffers(1, &m_Buffer);
			m_Buffer = u32max;
		}
	}

	u32 Buffer::frequency(void) const
	{
		int freq;

		alGetBufferi(m_Buffer, AL_FREQUENCY, &freq);

		return (u32)freq;
	}

	u32 Buffer::bit_depth(void) const
	{
		int bit_depth;

		alGetBufferi(m_Buffer, AL_BITS, &bit_depth);

		return (u32)bit_depth;
	}

	u32 Buffer::channel_count(void) const
	{
		int channel_count;

		alGetBufferi(m_Buffer, AL_CHANNELS, &channel_count);

		return (u32)channel_count;
	}

	u32 Buffer::size(void) const
	{
		int size;

		alGetBufferi(m_Buffer, AL_SIZE, &size);

		return (u32)size;
	}

	Buffer::Buffer(Buffer&& other) noexcept
	: m_Buffer(std::exchange(other.m_Buffer, u32max))
	{

	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->destroy();

		m_Buffer = std::exchange(other.m_Buffer, u32max);

		return *this;
	}
} // namespace Na::Audio
