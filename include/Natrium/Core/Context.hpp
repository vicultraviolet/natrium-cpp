#if !defined(NA_CONTEXT_HPP)
#define NA_CONTEXT_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Core/Event.hpp"

namespace Na {
	struct ContextInitInfo {
	};

	class Context {
	public:
		Context(void) = default;

		Context(const ContextInitInfo& info);
		~Context(void) { this->destroy(); }

		void destroy(void);

		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

		Context(Context&& other);
		Context& operator=(Context&& other);

		[[nodiscard]] static inline bool Exists(void) { return Context::s_Context; }
		[[nodiscard]] static inline Context& Get(void) { return *Context::s_Context; }

		[[nodiscard]] inline EventQueue& event_queue(void) { return m_EventQueue; }
		[[nodiscard]] inline const EventQueue& event_queue(void) const { return m_EventQueue; }

		[[nodiscard]] inline const std::filesystem::path& exec_path(void) const { return m_ExecPath; }
		[[nodiscard]] inline const std::filesystem::path& exec_dir(void)  const { return m_ExecDir; }
		[[nodiscard]] inline const std::filesystem::path& exec_name(void) const { return m_ExecName; }
	private:
		std::filesystem::path m_ExecPath, m_ExecDir, m_ExecName;
		std::string_view m_Version;

		EventQueue m_EventQueue;

		bool m_Valid = true;

		static inline Context* s_Context = nullptr;
	};
}

#endif // NA_CONTEXT_HPP