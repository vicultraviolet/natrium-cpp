#include "Pch.hpp"
#include "Natrium/Graphics/Device.hpp"

#include "Internal.hpp"
#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

namespace Na::Graphics {
	UniqueRef<Device> Device::Make(const DeviceInitInfo& info)
	{
		switch (info.backend)
		{
		case DeviceBackend::Vulkan: return MakeUnique<VulkanImpl::Device>(info);
		}
		return nullptr;
	}

	Device::Device(const DeviceInitInfo& info)
	: m_Backend(info.backend),
	  m_Extensions(info.required_extensions),
	  m_UniformIndexingInfo(info.uniform_indexing_info.value_or({}))
	{
		NA_VERIFY(!Device::s_Instance, "Failed to create Device: Device already initialized!");
		Device::s_Instance = this;
	}

	void Device::destroy(void)
	{
		m_Backend = DeviceBackend::None;

		Device::s_Instance = nullptr;
	}
} // namespace Na
