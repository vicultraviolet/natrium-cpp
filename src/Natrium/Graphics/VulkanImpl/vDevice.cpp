#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

#include "Natrium/Graphics/VulkanImpl/vShader.hpp"

#include "Internal.hpp"

namespace Na::VulkanImpl {
	static vk::DebugUtilsMessengerEXT createDbgMessenger(
		vk::Instance instance = {},
		const vk::DebugUtilsMessengerCreateInfoEXT& create_info = {}
	)
	{
		VkDebugUtilsMessengerEXT messenger = nullptr;

		auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (!fn)
			throw vk::ExtensionNotPresentError("Failed to create debug messenger!");

		VkResult result = fn(
			instance,
			&(create_info.operator const VkDebugUtilsMessengerCreateInfoEXT & ()),
			nullptr,
			&messenger
		);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create debug messenger!");

		return messenger;
	}

	static void destroyDbgMessenger(
		vk::Instance instance = {},
		vk::DebugUtilsMessengerEXT messenger = {}
	)
	{
		auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (!fn)
			throw vk::ExtensionNotPresentError("Failed to destroy debug messenger!");

		fn(
			instance,
			messenger,
			nullptr
		);
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL dbgCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT types,
		const VkDebugUtilsMessengerCallbackDataEXT* data,
		void* user_data
	)
	{
		if (!k_ValidationLayersEnabled)
			return VK_FALSE;

		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			g_Logger.print(Error, data->pMessage);
#if defined(_MSC_VER)
			__debugbreak();
#endif
		} else
			if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				g_Logger.print(Warn, data->pMessage);
			} else
				if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
				{
					g_Logger.print(Info, data->pMessage);
				} else
				{
					g_Logger.print(Trace, data->pMessage);
				}

				return VK_FALSE;
	}

	static vk::DebugUtilsMessengerCreateInfoEXT dbgMessengerInfo(void)
	{
		vk::DebugUtilsMessengerCreateInfoEXT info{};

		info.messageSeverity =
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;

		info.messageType =
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

		info.setPfnUserCallback(dbgCallback);

		return info;
	}

	static bool validationLayersSupported(const ArrayList<const char*>& requested_layers)
	{
		auto available_layers = vk::enumerateInstanceLayerProperties();
		if (available_layers.empty())
			return false;

		for (const char* requested_layer : requested_layers)
		{
			bool found = false;

			for (const vk::LayerProperties& available_layer : available_layers)
			{
				if (!strcmp(requested_layer, available_layer.layerName))
				{
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}

		return true;
	}

	const char* DeviceExtensionToVk(DeviceExtension extension)
	{
		switch (extension)
		{
		case DeviceExtension::Swapchain:	   return VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		case DeviceExtension::UniformIndexing: return VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME;
		}
		return "";
	}

	ArrayList<const char*> DeviceExtensionsToVk(const DeviceExtensions& extensions)
	{
		ArrayList<const char*> vk_extensions(extensions.size());

		for (const auto& extension : extensions)
		{
			const char* vk_extension = DeviceExtensionToVk(extension);
			if (strlen(vk_extension))
				vk_extensions.emplace_back(vk_extension);
		}

		return vk_extensions;
	}

	vk::PhysicalDeviceDescriptorIndexingFeaturesEXT
		UniformIndexingInfoToVk(const UniformIndexingInfo& info)
	{
		vk::PhysicalDeviceDescriptorIndexingFeaturesEXT features{};

		if (info.array_types.contains(UniformType::Texture))
			features.shaderSampledImageArrayNonUniformIndexing = true;

		if (info.array_types.contains(UniformType::UniformBuffer) ||
			info.array_types.contains(UniformType::UniformMultibuffer))
			features.shaderUniformBufferArrayNonUniformIndexing = true;

		if (info.array_types.contains(UniformType::StorageBuffer) || 
			info.array_types.contains(UniformType::StorageMultibuffer))
			features.shaderStorageBufferArrayNonUniformIndexing = true;

		if (info.update_after_bind_types.contains(UniformType::Texture))
			features.shaderSampledImageArrayNonUniformIndexing = true;

		if (info.update_after_bind_types.contains(UniformType::UniformBuffer) ||
			info.update_after_bind_types.contains(UniformType::UniformMultibuffer))
			features.shaderUniformBufferArrayNonUniformIndexing = true;

		if (info.update_after_bind_types.contains(UniformType::StorageBuffer) ||
			info.update_after_bind_types.contains(UniformType::StorageMultibuffer))
			features.shaderStorageBufferArrayNonUniformIndexing = true;

		features.descriptorBindingPartiallyBound = info.binding_partially_bound;

		features.runtimeDescriptorArray = info.runtime_array;

		features.descriptorBindingUpdateUnusedWhilePending = info.update_while_in_use;

		features.descriptorBindingVariableDescriptorCount = info.dynamic_count;

		return features;
	}

	static bool areRequiredExtensionsSupported(
		vk::PhysicalDevice device,
		const Na::ArrayList<const char*>& extensions
	)
	{
		auto available_extensions = device.enumerateDeviceExtensionProperties();
		std::set<std::string_view> required_extensions(extensions.begin(), extensions.end());

		for (const auto& extension : available_extensions)
			required_extensions.erase(extension.extensionName);

		return required_extensions.empty();
	}

	static i32 ratePhysicalDevice(
		vk::PhysicalDevice device,
		vk::SurfaceKHR surface,
		const Na::ArrayList<const char*>& extensions
	)
	{
		if (!device)
			return 0;

		vk::PhysicalDeviceProperties properties = device.getProperties();
		vk::PhysicalDeviceFeatures features = device.getFeatures();

		if (!features.geometryShader)
			return 0;

		if (!features.samplerAnisotropy)
			return 0;

		if (!Internal::QueueFamilyIndices(device, surface))
			return 0;

		if (!areRequiredExtensionsSupported(device, extensions))
			return 0;

		if (!Internal::SurfaceSupport(device, surface))
			return 0;

		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			properties.limits.maxImageDimension2D += 1000;

		return properties.limits.maxImageDimension2D;
	}

	Device::Device(const DeviceInitInfo& info)
	: Graphics::Device(info)
	{
		ArrayList<const char*> device_extensions = DeviceExtensionsToVk(info.required_extensions);

		if (info.required_extensions.contains(DeviceExtension::Swapchain))
		{
			device_extensions.emplace_back(VK_KHR_MAINTENANCE_1_EXTENSION_NAME);
		}

		this->_create_instance();
		this->_create_dbg_messenger();

		GLFWwindow* temp_window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
		vk::SurfaceKHR temp_surface = Internal::CreateWindowSurface(temp_window);

		this->_pick_physical_device(temp_surface, device_extensions);
		this->_get_limits();

		if (info.uniform_indexing_info.has_value() &&
			info.required_extensions.contains(DeviceExtension::UniformIndexing))
		{
			auto uniform_indexing_features = UniformIndexingInfoToVk(
				info.uniform_indexing_info.value()
			);

			this->_create_logical_device(
				temp_surface,
				device_extensions,
				&uniform_indexing_features
			);
		} else
		{
			this->_create_logical_device(
				temp_surface,
				device_extensions,
				nullptr // uniform indexing features (not used)
			);
		}

		this->_create_single_time_cmd_pool();

		m_Instance.destroySurfaceKHR(temp_surface);
		glfwDestroyWindow(temp_window);
	}

	void Device::destroy(void)
	{
		if (m_SingleTimeCommandPool)
		{
			m_LogicalDevice.destroyCommandPool(m_SingleTimeCommandPool);
			m_SingleTimeCommandPool = nullptr;
		}

		m_GraphicsQueueIndex = u32max;
		m_GraphicsQueue = nullptr;

		if (m_LogicalDevice)
		{
			m_LogicalDevice.destroy();
			m_LogicalDevice = nullptr;
		}

		if (m_DebugMessenger)
		{
			destroyDbgMessenger(m_Instance, m_DebugMessenger);
			m_DebugMessenger = nullptr;
		}

		if (m_Instance)
		{
			m_Instance.destroy();
			m_Instance = nullptr;
		}

		Graphics::Device::destroy();
	}

	void Device::_create_instance(void)
	{
		const ArrayList<const char*> validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};

		if (k_ValidationLayersEnabled && !validationLayersSupported(validation_layers))
			throw std::runtime_error("Failed to initialize Device: Validation layers requested, but not available!");

		vk::ApplicationInfo app_info;
		app_info.apiVersion = VK_API_VERSION_1_0;

		app_info.pEngineName = "Natrium";
		app_info.pApplicationName = "Natrium Application";

		vk::InstanceCreateInfo create_info;
		create_info.pApplicationInfo = &app_info;

		u32 glfw_extension_count = 0;
		const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		ArrayList<const char*> instance_extensions(glfw_extension_count + 1llu);
		for (u32 i = 0; i < glfw_extension_count; i++)
			instance_extensions.emplace(glfw_extensions[i]);

		vk::DebugUtilsMessengerCreateInfoEXT debug_create_info = dbgMessengerInfo();
		if (k_ValidationLayersEnabled)
		{
			instance_extensions.emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			create_info.enabledLayerCount = (u32)validation_layers.size();
			create_info.ppEnabledLayerNames = validation_layers.ptr();

			create_info.pNext = &debug_create_info;
		}

		create_info.enabledExtensionCount = (u32)instance_extensions.size();
		create_info.ppEnabledExtensionNames = instance_extensions.ptr();

		m_Instance = vk::createInstance(create_info);
	}

	void Device::_create_dbg_messenger(void)
	{
		if (!k_ValidationLayersEnabled)
			return;

		m_DebugMessenger = createDbgMessenger(m_Instance, dbgMessengerInfo());
	}

	void Device::_pick_physical_device(
		vk::SurfaceKHR surface,
		const Na::ArrayList<const char*>& extensions
	)
	{
		i32 high_score = 0;
		for (const auto& device : m_Instance.enumeratePhysicalDevices())
		{
			i32 score = ratePhysicalDevice(device, surface, extensions);
			if (score > high_score)
			{
				high_score = score;
				m_PhysicalDevice = device;
			}
		}
	}

	void Device::_get_limits(void)
	{
		static constexpr std::array<vk::SampleCountFlagBits, 7> x_SampleCounts =
		{
			vk::SampleCountFlagBits::e64,
			vk::SampleCountFlagBits::e32,
			vk::SampleCountFlagBits::e16,
			vk::SampleCountFlagBits::e8,
			vk::SampleCountFlagBits::e4,
			vk::SampleCountFlagBits::e2,
			vk::SampleCountFlagBits::e1
		};

		vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties();

		vk::SampleCountFlags sample_counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;

		for (vk::SampleCountFlagBits sample_count : x_SampleCounts)
		{
			if ((sample_counts & sample_count) == sample_count)
			{
				m_Limits.vk_msaa_sample_count = sample_count;
				break;
			}
		}

		m_Limits.vk_max_anisotropy = properties.limits.maxSamplerAnisotropy;
	}

	void Device::_create_logical_device(
		vk::SurfaceKHR surface,
		const Na::ArrayList<const char*>& extensions,
		vk::PhysicalDeviceDescriptorIndexingFeaturesEXT* descriptor_indexing_features
	)
	{
		Internal::QueueFamilyIndices queue_indices(m_PhysicalDevice, surface);

		std::array<float, 1> priorities = { 1.0f };

		ArrayList<vk::DeviceQueueCreateInfo> queue_create_infos;
		queue_create_infos.emplace(
			vk::DeviceQueueCreateFlags{ 0U },
			queue_indices.graphics(),
			(u32)priorities.size(),
			priorities.data()
		);

		vk::DeviceCreateInfo create_info;

		create_info.queueCreateInfoCount = (u32)queue_create_infos.size();
		create_info.pQueueCreateInfos = queue_create_infos.ptr();

		vk::PhysicalDeviceFeatures device_features{};
		device_features.samplerAnisotropy = VK_TRUE;
		device_features.sampleRateShading = VK_TRUE;
		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = (u32)extensions.size();
		create_info.ppEnabledExtensionNames = extensions.ptr();

		create_info.pNext = descriptor_indexing_features;

		m_LogicalDevice = m_PhysicalDevice.createDevice(create_info);

		m_GraphicsQueue = m_LogicalDevice.getQueue(queue_indices.graphics(), 0);
		m_GraphicsQueueIndex = queue_indices.graphics();
	}

	void Device::_create_single_time_cmd_pool(void)
	{
		vk::CommandPoolCreateInfo create_info;
		create_info.queueFamilyIndex = m_GraphicsQueueIndex;
		create_info.flags = vk::CommandPoolCreateFlagBits::eTransient;

		m_SingleTimeCommandPool = m_LogicalDevice.createCommandPool(create_info);
	}

	MSAASampleCount DeviceLimits::msaa_sample_count(void) const
	{
		switch (this->vk_msaa_sample_count)
		{
		case vk::SampleCountFlagBits::e1:  return MSAASampleCount::x1;
		case vk::SampleCountFlagBits::e2:  return MSAASampleCount::x2;
		case vk::SampleCountFlagBits::e4:  return MSAASampleCount::x4;
		case vk::SampleCountFlagBits::e8:  return MSAASampleCount::x8;
		case vk::SampleCountFlagBits::e16: return MSAASampleCount::x16;
		case vk::SampleCountFlagBits::e32: return MSAASampleCount::x32;
		case vk::SampleCountFlagBits::e64: return MSAASampleCount::x64;
		}
		return MSAASampleCount::None;
	}

	float DeviceLimits::max_anisotropy(void) const
	{
		return this->vk_max_anisotropy;
	}

	vk::SampleCountFlagBits DeviceLimits::vk_msaa_sample_count_if(bool enabled) const
	{
		if (!enabled)
			return vk::SampleCountFlagBits::e1;

		return this->vk_msaa_sample_count;
	}
} // namespace Na::VulkanImpl