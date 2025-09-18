#if !defined(NA_AUDIO_BUFFER_HPP)
#define NA_AUDIO_BUFFER_HPP

#include "Natrium/Audio/Wav.hpp"

namespace Na::Audio {
	class Buffer {
	public:
		Buffer(void) = default;

		void destroy(void);
		~Buffer(void) { this->destroy(); }

		explicit Buffer(WeakRef<Wav> wav);

		Buffer(const Buffer& other) = default;
		Buffer& operator=(const Buffer& other) = default;

		Buffer(Buffer&& other) noexcept;
		Buffer& operator=(Buffer&& other) noexcept;

		[[nodiscard]] u32 frequency(void) const;
		[[nodiscard]] u32 bit_depth(void) const;
		[[nodiscard]] u32 channel_count(void) const;
		[[nodiscard]] u32 size(void) const;

		[[nodiscard]] inline ALuint native(void) const { return m_Buffer; }

		[[nodiscard]] inline operator bool(void) const { return m_Buffer != u32max; }
		[[nodiscard]] inline bool operator==(Buffer buffer) { return m_Buffer == buffer.m_Buffer; }
	private:
		ALuint m_Buffer = u32max;
	};
} // namespace Na::Audio

#endif // NA_AUDIO_BUFFER_HPP