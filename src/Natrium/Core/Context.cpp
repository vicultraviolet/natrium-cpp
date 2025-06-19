#include "Pch.hpp"
#include "Natrium/Core/Context.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#include <Windows.h>
#endif // NA_PLATFORM_WINDOWS

#include <GLFW/glfw3.h>

namespace Na {
	static std::filesystem::path getExecPath(void)
	{
	#if defined(NA_PLATFORM_LINUX)
		return std::filesystem::canonical("/proc/self/exe");
	#elif defined(NA_PLATFORM_WINDOWS)
		char exec_path_buffer[MAX_PATH];
		GetModuleFileNameA(nullptr, exec_path_buffer, MAX_PATH);
		return exec_path_buffer;
	#else
		return "";
	#endif // NA_PLATFORM
	}

	Context::Context(const ContextInitInfo& info)
	: m_ExecPath(getExecPath()),
	m_ExecDir(m_ExecPath.parent_path()),
	m_ExecName(m_ExecPath.filename()),
	m_Version("Pre-Alpha")
	{
		NA_VERIFY(!Context::s_Context, "Failed to create Context: Cannot create more than one Context!");
		Context::s_Context = this;

		g_Logger.print_header();
		g_Logger.printf(Info, "Initializing Natrium version {}", m_Version);

		glfwSetErrorCallback([](int error, const char* description)
		{
			if (error == 65539)
				return;

			g_Logger.printf(Error, "GLFW Error#{}: {}", error, description);
			throw std::runtime_error(NA_FORMAT("GLFW Error #{}", error));
		});
		int result = glfwInit();
		NA_ASSERT(result, "Failed to initialize glfw!");
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void Context::destroy(void)
	{
		if (!m_Valid)
			return;
		m_Valid = false;

		g_Logger.print(Info, "Shutting down Natrium, Goodbye!");

		glfwTerminate();

		s_Context = nullptr;
	}

	Context::Context(Context&& other)
	: m_ExecPath(std::move(other.m_ExecPath)),
	m_ExecDir(std::move(other.m_ExecDir)),
	m_ExecName(std::move(other.m_ExecName)),
	m_Version(std::move(other.m_Version)),
	m_EventQueue(std::move(other.m_EventQueue)),
	m_Valid(std::exchange(other.m_Valid, false))
	{
		Context::s_Context = this;
	}

	Context& Context::operator=(Context&& other)
	{
		m_ExecPath = std::move(other.m_ExecPath);
		m_ExecDir = std::move(other.m_ExecDir);
		m_ExecName = std::move(other.m_ExecName);
		m_Version = std::move(other.m_Version);
		m_EventQueue = std::move(other.m_EventQueue);
		m_Valid = std::exchange(other.m_Valid, false);

		Context::s_Context = this;
		return *this;
	}
} // namespace Na
