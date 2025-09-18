#include "Pch.hpp"
#include "Natrium/Audio/Wav.hpp"

namespace Na::Audio {
	FileErrorCode Wav::load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			return FileErrorCode::NotFound;
		}

		std::string path_str = path.string();

		SF_INFO file_info;
		SNDFILE* file = sf_open(path_str.c_str(), SFM_READ, &file_info);

		if (!file)
			return FileErrorCode::Unknown;

		int type = file_info.format & SF_FORMAT_TYPEMASK;
		int subtype = file_info.format & SF_FORMAT_SUBMASK;

		m_ChannelCount = file_info.channels;

		m_SampleRate = file_info.samplerate;
		m_SampleCount = (u64)file_info.frames;

		m_Duration = (float)m_SampleCount / (float)m_SampleRate;

		switch (subtype)
		{
		case SF_FORMAT_PCM_U8:
		case SF_FORMAT_PCM_S8:
			m_BitDepth = 8;
			break;

		case SF_FORMAT_PCM_16:
		case SF_FORMAT_PCM_24:
		case SF_FORMAT_PCM_32:
			m_BitDepth = 16;
			break;
		}

		u64 total_sample_count = (u64)m_SampleCount * (u64)m_ChannelCount;

		ArrayList<float> samples(total_sample_count, total_sample_count);
		sf_readf_float(file, samples.ptr(), file_info.frames);

		sf_close(file);

		u64 size_in_bytes = total_sample_count * m_BitDepth / 8;
		m_Data.reallocate(size_in_bytes, size_in_bytes);

		if (m_BitDepth == 8)
		{
			for (u64 i = 0; float sample : samples)
				m_Data[i++] = glm::packSnorm1x8(sample);

		} else
		if (m_BitDepth == 16)
		{
			ArrayList<u16> data(total_sample_count, total_sample_count);

			for (u64 i = 0; float sample : samples)
				data[i++] = glm::packSnorm1x16(sample);

			memcpy(m_Data.ptr(), data.ptr(), size_in_bytes);
		}

		g_Logger.print(Debug, "Loading audio file: [");

		g_Logger.printf(Debug, "\tpath: {}",          path_str);
		g_Logger.printf(Debug, "\tchannel count: {}", m_ChannelCount);
		g_Logger.printf(Debug, "\tsample rate: {}",   m_SampleRate);
		g_Logger.printf(Debug, "\tbit depth: {}",     m_BitDepth);
		g_Logger.printf(Debug, "\tsample count: {}",  m_SampleCount);
		g_Logger.printf(Debug, "\tduration: {:.3f}s", m_Duration);

		g_Logger.print(Debug, "]");

		return FileErrorCode::None;
	}
} // namespace Na::Audio
