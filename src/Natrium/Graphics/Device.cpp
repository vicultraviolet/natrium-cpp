#include "Pch.hpp"
#include "Natrium/Graphics/Device.hpp"

#include "Internal.hpp"

namespace Na {
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT types,
		const VkDebugUtilsMessengerCallbackDataEXT* data,
		void* user_data
	)
	{
		if (k_ValidationLayersEnabled)
		{
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
		}

		return VK_FALSE;
	}

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
			&(create_info.operator const VkDebugUtilsMessengerCreateInfoEXT& ()),
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

		info.setPfnUserCallback(debugCallback);

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

	static bool areRequiredExtensionsSupported(vk::PhysicalDevice device, const Na::ArrayList<const char*>& extensions)
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

	void Device::Initialize(const DeviceInitInfo& info)
	{
		if (Internal::g_DeviceData)
			Device::Shutdown();

		Na::ArrayList<const char*> device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_MAINTENANCE1_EXTENSION_NAME
		};

		Device::_CreateInstance(info.app_name.data());
		Device::_CreateDebugMessenger();

		GLFWwindow* temp_window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
		vk::SurfaceKHR temp_surface = Internal::CreateWindowSurface(temp_window);

		Device::_PickPhysicalDevice(temp_surface, device_extensions);
		Device::_GetLimits();

		Device::_CreateLogicalDevice(temp_surface, device_extensions);
		Device::_CreateSingleTimeCommandPool();

		Internal::g_DeviceData.instance.destroySurfaceKHR(temp_surface);
		glfwDestroyWindow(temp_window);
	}

	void Device::Shutdown(void)
	{
		Internal::DeviceData& data = Internal::g_DeviceData;

		Device::Wait();

		if (data.single_time_cmd_pool)
		{
			data.logical_device.destroyCommandPool(data.single_time_cmd_pool);
			data.single_time_cmd_pool = nullptr;
		}

		data.graphics_queue_index = k_U32Max;
		data.graphics_queue = nullptr;

		if (data.logical_device)
		{
			data.logical_device.destroy();
			data.logical_device = nullptr;
		}

		if (data.dbg_messenger)
		{
			destroyDbgMessenger(data.instance, data.dbg_messenger);
			data.dbg_messenger = nullptr;
		}

		if (data.instance)
		{
			data.instance.destroy();
			data.instance = nullptr;
		}
	}

	void Device::Wait(void)
	{
		Internal::g_DeviceData.logical_device.waitIdle();
	}

	bool Device::Initialized(void)
	{
		return Internal::g_DeviceData;
	}

	vk::SampleCountFlagBits DeviceLimits::MSAASampleCount(void)
	{
		return Internal::g_DeviceData.limits.msaa_sample_count;
	}

	float DeviceLimits::Anisotropy(void)
	{
		return Internal::g_DeviceData.limits.anisotropy;
	}

	void Device::_CreateInstance(const char* app_name)
	{
		const ArrayList<const char*> validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};

		if (k_ValidationLayersEnabled && !validationLayersSupported(validation_layers))
			throw std::runtime_error("Failed to initialize Device: Validation layers requested, but not available!");

		vk::ApplicationInfo app_info;
		app_info.apiVersion = VK_API_VERSION_1_0;

		app_info.pEngineName = "Natrium";
		app_info.pApplicationName = app_name;

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

		Internal::g_DeviceData.instance = vk::createInstance(create_info);
	}

	void Device::_CreateDebugMessenger(void)
	{
		if (!k_ValidationLayersEnabled)
			return;

		Internal::DeviceData& data = Internal::g_DeviceData;
		data.dbg_messenger = createDbgMessenger(data.instance, dbgMessengerInfo());
	}

	void Device::_PickPhysicalDevice(vk::SurfaceKHR surface, const Na::ArrayList<const char*>& extensions)
	{
		i32 high_score = 0;
		for (const auto& device : Internal::g_DeviceData.instance.enumeratePhysicalDevices())
		{
			i32 score = ratePhysicalDevice(device, surface, extensions);
			if (score > high_score)
			{
				high_score = score;
				Internal::g_DeviceData.physical_device = device;
			}
		}
	}

	void Device::_GetLimits(void)
	{
		Internal::DeviceData& data = Internal::g_DeviceData;

		static constexpr std::array<vk::SampleCountFlagBits, 7> x_SampleCounts = {
			vk::SampleCountFlagBits::e64,
			vk::SampleCountFlagBits::e32,
			vk::SampleCountFlagBits::e16,
			vk::SampleCountFlagBits::e8,
			vk::SampleCountFlagBits::e4,
			vk::SampleCountFlagBits::e2,
			vk::SampleCountFlagBits::e1
		};

		vk::PhysicalDeviceProperties properties = data.physical_device.getProperties();

		vk::SampleCountFlags sample_counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;

		for (vk::SampleCountFlagBits sample_count : x_SampleCounts)
		{
			if ((sample_counts & sample_count) == sample_count)
			{
				data.limits.msaa_sample_count = sample_count;
				break;
			}
		}

		data.limits.anisotropy = properties.limits.maxSamplerAnisotropy;
	}

	void Device::_CreateLogicalDevice(vk::SurfaceKHR surface, const Na::ArrayList<const char*>& extensions)
	{
		Internal::DeviceData& data = Internal::g_DeviceData;

		Internal::QueueFamilyIndices queue_indices(data.physical_device, surface);

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

		data.logical_device = data.physical_device.createDevice(create_info);
		data.graphics_queue = data.logical_device.getQueue(queue_indices.graphics(), 0);
		data.graphics_queue_index = queue_indices.graphics();
	}

	void Device::_CreateSingleTimeCommandPool(void)
	{
		Internal::DeviceData& data = Internal::g_DeviceData;

		vk::CommandPoolCreateInfo create_info;
		create_info.queueFamilyIndex = data.graphics_queue_index;
		create_info.flags = vk::CommandPoolCreateFlagBits::eTransient;

		data.single_time_cmd_pool = data.logical_device.createCommandPool(create_info);
	}
} // namespace Na
