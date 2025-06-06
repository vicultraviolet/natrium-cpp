#include "Pch.hpp"
#include "Natrium/Core/DynamicLibrary.hpp"

#include "Natrium/Core/Context.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NA_PLATFORM_LINUX)
#include <dlfcn.h>
#endif // NA_PLATFORM_WINDOWS

namespace Na {
	void DynamicLibrary::load(const std::string_view& name)
	{
		if (name.empty())
			return;

		if (m_Handle)
			this->unload();

		m_Name = name;

		g_Logger.printf(Trace, "Loading library {}", name);
		std::filesystem::path dir = Context::GetExecDir();
	#if defined(NA_PLATFORM_WINDOWS)
		dir = (dir / name).replace_extension(".dll");
		m_Handle = LoadLibraryW(dir.c_str());
	#elif defined(NA_PLATFORM_LINUX)
		dir = (dir / (std::string("lib") + std::string(name))).replace_extension(".so");
		m_Handle = dlopen(dir.c_str(), RTLD_LAZY);
	#endif

		NA_ASSERT(m_Handle, "Failed to load library {}", dir.string());
	}

	void DynamicLibrary::unload(void)
	{
		if (!m_Handle)
			return;

		g_Logger.printf(Trace, "Unloading library {}", m_Name);
	#if defined(NA_PLATFORM_WINDOWS)
		FreeLibrary((HMODULE)m_Handle);
	#elif defined(NA_PLATFORM_LINUX)
		dlclose(m_Handle);
	#endif
		m_Handle = nullptr;
		m_Name = "";
	}

	void* DynamicLibrary::fn(const char* name) const
	{
		if (!m_Handle)
			return nullptr;

	#if defined(NA_PLATFORM_WINDOWS)
		return GetProcAddress((HMODULE)m_Handle, name);
	#elif defined(NA_PLATFORM_LINUX)
		return dlsym(m_Handle, name);
	#endif
		return nullptr;
	}

	DynamicLibrary::DynamicLibrary(DynamicLibrary&& other)
	: m_Handle(std::exchange(other.m_Handle, nullptr)),
	m_Name(std::move(other.m_Name))
	{}

	DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other)
	{
		this->unload();
		m_Handle = std::exchange(other.m_Handle, nullptr);
		m_Name = std::move(other.m_Name);
		return *this;
	}
} // namespace Na
