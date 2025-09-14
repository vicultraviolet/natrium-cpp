#if !defined(NA_AUDIO_CONTEXT_HPP)
#define NA_AUDIO_CONTEXT_HPP

#include "Natrium/Core.hpp"

namespace Na::Audio {
	struct ContextInitInfo {

	};

	class Context {
	public:
		Context(void) = default;
		~Context(void) { this->destroy(); }

		Context(const ContextInitInfo& info);
		void destroy(void);

		Context(Context&& other) noexcept;
		Context& operator=(Context&& other) noexcept;

		[[nodiscard]] inline ALCdevice* device(void) { return m_Device; }
		[[nodiscard]] inline const ALCdevice* device(void) const { return m_Device; }

		[[nodiscard]] inline ALCcontext* context(void) { return m_Context; }
		[[nodiscard]] inline const ALCcontext* context(void) const { return m_Context; }
	private:
		ALCdevice* m_Device = nullptr;
		ALCcontext* m_Context = nullptr;
	};
} // namespace Na::Audio

#endif // NA_AUDIO_CONTEXT_HPP