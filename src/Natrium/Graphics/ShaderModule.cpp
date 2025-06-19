#include "Pch.hpp"
#include "Natrium/Graphics/ShaderModule.hpp"

#include "Internal.hpp"

namespace Na {
	ShaderModule::ShaderModule(
		const ShaderBinary& binary,
		ShaderStageBits stage,
		const std::string_view& entry_point
	)
	: m_Module(Internal::g_DeviceData.logical_device
			   .createShaderModule(vk::ShaderModuleCreateInfo(
				   {},
				   binary.size() * sizeof(u32),
				   binary.ptr()
			   ))),
	m_Stage(stage),
	m_EntryPoint(entry_point)
	{}

	ShaderModule::~ShaderModule(void)
	{
		Internal::g_DeviceData.logical_device.destroyShaderModule(m_Module);
	}

	ShaderModule::ShaderModule(ShaderModule&& other)
	: m_Module(std::exchange(other.m_Module, nullptr)),
	m_Stage(std::move(other.m_Stage)),
	m_EntryPoint(std::move(other.m_EntryPoint))
	{}

	ShaderModule& ShaderModule::operator=(ShaderModule&& other)
	{
		Internal::g_DeviceData.logical_device.destroyShaderModule(m_Module);
		m_Module = std::exchange(other.m_Module, nullptr);
		m_Stage = std::move(other.m_Stage);
		m_EntryPoint = std::move(other.m_EntryPoint);
		return *this;
	}
} // namespace Na
