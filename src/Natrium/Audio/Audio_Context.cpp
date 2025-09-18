#include "Pch.hpp"
#include "Natrium/Audio/Audio_Context.hpp"

namespace Na::Audio {
	Context::Context(const ContextInitInfo& info)
	{
		m_Device = alcOpenDevice(nullptr);

		m_Context = alcCreateContext(m_Device, nullptr);
		alcMakeContextCurrent(m_Context);
	}

	void Context::destroy(void)
	{
		if (m_Context)
		{
			alcMakeContextCurrent(nullptr);
			alcDestroyContext(m_Context);
		}

		if (m_Device)
		{
			alcCloseDevice(m_Device);
			m_Device = nullptr;
		}
	}

	void Context::bind(void)
	{
		Context::s_Instance = this;
	}

	View<Context> Context::GetBound(void)
	{
		return Context::s_Instance;
	}

	Context::Context(Context&& other) noexcept
	: m_Context(std::exchange(other.m_Context, nullptr)),
	  m_Device(std::exchange(other.m_Device, nullptr))
	{

	}

	Context& Context::operator=(Context&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->destroy();

		m_Context = std::exchange(other.m_Context, nullptr);
		m_Device = std::exchange(other.m_Device, nullptr);

		alcMakeContextCurrent(m_Context);

		return *this;
	}
} // namespace Na::Audio
