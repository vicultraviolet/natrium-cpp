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

	Context::Context(const std::filesystem::path& exec_path, const std::string_view& version)
	: m_ExecPath(exec_path),
	m_ExecDir(exec_path.parent_path()),
	m_ExecName(exec_path.filename()),
	m_Version(version)
	{

	}

	Context::Context(Context&& other)
	: m_ExecPath(std::move(other.m_ExecPath)),
	m_ExecDir(std::move(other.m_ExecDir)),
	m_ExecName(std::move(other.m_ExecName)),
	m_Version(std::move(other.m_Version)),
	m_EventQueue(std::move(other.m_EventQueue)),
	m_VkContext(std::move(other.m_VkContext))
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
		m_VkContext = std::move(other.m_VkContext);

		Context::s_Context = this;
		return *this;
	}

	Context Context::Initialize(void)
	{
		Context context(getExecPath(), "Pre-Alpha");

		g_Logger.print_header();
		g_Logger.printf(Info, "Initializing Natrium version {}", context.m_Version);

		glfwSetErrorCallback([](int error, const char* description)
		{
			g_Logger.printf(Error, "GLFW Error#{}: {}", error, description);
			throw std::runtime_error(NA_FORMAT("GLFW Error #{}", error));
		});
		int result = glfwInit();
		NA_ASSERT(result, "Failed to initialize glfw!");
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		context.m_VkContext = VkContext::Initialize();

		s_Context = &context;
		return context;
	}

	void Context::Shutdown(void)
	{
		g_Logger.print(Info, "Shutting down Natrium, Goodbye!");

		VkContext::Shutdown();
		glfwTerminate();

		s_Context = nullptr;
	}
} // namespace Na
