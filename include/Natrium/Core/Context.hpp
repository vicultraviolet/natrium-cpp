#if !defined(NA_CONTEXT_HPP)
#define NA_CONTEXT_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Core/Event.hpp"
#include "Natrium/Graphics/VkContext.hpp"

namespace Na {
	class Context {
	public:
		Context(void) = default;

		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

		Context(Context&& other);
		Context& operator=(Context&& other);

		static Context Initialize(void);
		static void Shutdown(void);

		static EventQueue& GetEventQueue(void) { return s_Context->m_EventQueue; }

		static const std::filesystem::path& GetExecPath(void) { return s_Context->m_ExecPath; }
		static const std::filesystem::path& GetExecDir(void)  { return s_Context->m_ExecDir; }
		static const std::filesystem::path& GetExecName(void) { return s_Context->m_ExecName; }
	private:
		Context(const std::filesystem::path& exec_path, const std::string_view& version);
	private:
		std::filesystem::path m_ExecPath, m_ExecDir, m_ExecName;
		std::string_view m_Version;

		EventQueue m_EventQueue;

		VkContext m_VkContext;

		static inline Context* s_Context = nullptr;
	};
}

#endif // NA_CONTEXT_HPP