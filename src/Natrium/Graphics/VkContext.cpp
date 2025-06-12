#include "Pch.hpp"
#include "Natrium/Graphics/VkContext.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT types,
	const VkDebugUtilsMessengerCallbackDataEXT* data,
	void* user_data
)
{
	if (Na::k_ValidationLayersEnabled)
	{
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			Na::g_Logger.print(Na::Error, data->pMessage);
		#if defined(_MSC_VER)
			__debugbreak();
		#endif
		} else
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			Na::g_Logger.print(Na::Warn, data->pMessage);
		else
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			Na::g_Logger.print(Na::Info, data->pMessage);
		else
			Na::g_Logger.print(Na::Trace, data->pMessage);
	}

	return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;

}
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func)
		func(instance, debugMessenger, pAllocator);
}

namespace Na {
	static Na::ArrayList<const char*> extensions;
	static const Na::ArrayList<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	static const Na::ArrayList<const char*> requiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_MAINTENANCE1_EXTENSION_NAME
		//VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME
	};

	vk::SurfaceKHR createWindowSurface(GLFWwindow* window)
	{
		VkSurfaceKHR surface;
		VkResult result = glfwCreateWindowSurface(VkContext::Get().instance(), window, nullptr, &surface);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface!");
		return surface;
	}

	static void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info)
	{
		create_info.messageSeverity =
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;

		create_info.messageType =
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

		create_info.setPfnUserCallback(debugCallback);
	}

	static bool validationLayersSupported(void)
	{
		auto available_layers = vk::enumerateInstanceLayerProperties();

		for (const char* requested_layer : validationLayers)
		{
			bool found = false;

			for (const vk::LayerProperties& available_layer : available_layers)
				if (!strcmp(requested_layer, available_layer.layerName))
				{
					found = true;
					break;
				}

			if (!found)
				return false;
		}

		return true;
	}

	QueueFamilyIndices QueueFamilyIndices::Get(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		auto properties = device.getQueueFamilyProperties();

		QueueFamilyIndices indices;
		for (u32 i = 0; const auto& property : properties)
		{
			if (property.queueFlags & vk::QueueFlagBits::eGraphics)
				if (device.getSurfaceSupportKHR(i, surface))
					indices.graphics = i;

			if (indices)
				break;

			i++;
		}

		return indices;
	}

	static bool areRequiredDeviceExtensionsSupported(vk::PhysicalDevice device)
	{
		auto available_extensions = device.enumerateDeviceExtensionProperties();
        std::set<std::string_view> required_extensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

        for (const auto& extension : available_extensions)
            required_extensions.erase(extension.extensionName);

		return required_extensions.empty();
	}

	SurfaceSupport SurfaceSupport::Get(vk::SurfaceKHR surface, vk::PhysicalDevice device)
	{
		SurfaceSupport support{};

		support.capabilities = device.getSurfaceCapabilitiesKHR(surface);

		u32 format_count;
		(void)device.getSurfaceFormatsKHR(surface, &format_count, nullptr);
		support.formats.reallocate(format_count, format_count);
		(void)device.getSurfaceFormatsKHR(surface, &format_count, support.formats.ptr());

		u32 present_mode_count;
		(void)device.getSurfacePresentModesKHR(surface, &present_mode_count, nullptr);
		support.present_modes.reallocate(present_mode_count, present_mode_count);
		(void)device.getSurfacePresentModesKHR(surface, &present_mode_count, support.present_modes.ptr());

		return support;
	}

	static i32 ratePhysicalDevice(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		if (!device)
			return 0;

		vk::PhysicalDeviceProperties properties = device.getProperties();
		vk::PhysicalDeviceFeatures features = device.getFeatures();

		if (!features.geometryShader)
			return 0;

		if (!features.samplerAnisotropy)
			return 0;

		if (!QueueFamilyIndices::Get(device, surface))
			return 0;

		if (!areRequiredDeviceExtensionsSupported(device))
			return 0;

		if (!SurfaceSupport::Get(surface, device))
			return 0;

		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			properties.limits.maxImageDimension2D += 1000;

		return properties.limits.maxImageDimension2D;
	}

	static vk::Instance createInstance(void)
	{
		if (k_ValidationLayersEnabled && !validationLayersSupported())
			throw std::runtime_error("Validation layers requested, but not supported!");

		vk::ApplicationInfo app_info;
		app_info.apiVersion = VK_API_VERSION_1_0;

		vk::InstanceCreateInfo create_info;
		create_info.pApplicationInfo = &app_info;

		u32 required_extension_count = 0;
		const char** required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);

		extensions.reallocate(required_extension_count);
		for (u32 i = 0; i < required_extension_count; i++)
			extensions.emplace(required_extensions[i]);

		vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
		if (k_ValidationLayersEnabled)
		{
			extensions.emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			create_info.enabledLayerCount = (u32)validationLayers.size();
			create_info.ppEnabledLayerNames = validationLayers.ptr();

			populateDebugMessengerCreateInfo(debug_create_info);
			create_info.pNext = &debug_create_info;
		} else
		{
			create_info.enabledLayerCount = 0;
		}

		create_info.enabledExtensionCount = (u32)extensions.size();
		create_info.ppEnabledExtensionNames = extensions.ptr();

		return vk::createInstance(create_info);
	}

	static vk::DebugUtilsMessengerEXT createDebugMessenger(vk::Instance instance)
	{
		if (!k_ValidationLayersEnabled)
			return nullptr;

		vk::DebugUtilsMessengerCreateInfoEXT create_info;
		populateDebugMessengerCreateInfo(create_info);

		VkDebugUtilsMessengerEXT messenger;
		VkResult result = CreateDebugUtilsMessengerEXT(instance, (VkDebugUtilsMessengerCreateInfoEXT*)&create_info, nullptr, &messenger);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create vulkan debug messenger!");
		return messenger;
	}

	static vk::PhysicalDevice pickPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface)
	{
		auto devices = instance.enumeratePhysicalDevices();

		vk::PhysicalDevice chosen_device = nullptr;

		i32 high_score = 0;
		for (const auto& device : devices)
		{
			i32 score = ratePhysicalDevice(device, surface);
			if (score > high_score)
			{
				high_score = score;
				chosen_device = device;
			}
		}

		return chosen_device;
	}

	static vk::SampleCountFlagBits getMaxSampleCount(vk::PhysicalDevice physical_device)
	{
		vk::PhysicalDeviceProperties properties = physical_device.getProperties();
		vk::SampleCountFlags counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;

		if (counts & vk::SampleCountFlagBits::e64) return vk::SampleCountFlagBits::e64;
		if (counts & vk::SampleCountFlagBits::e32) return vk::SampleCountFlagBits::e32;
		if (counts & vk::SampleCountFlagBits::e16) return vk::SampleCountFlagBits::e16;
		if (counts & vk::SampleCountFlagBits::e8)  return vk::SampleCountFlagBits::e8;
		if (counts & vk::SampleCountFlagBits::e4)  return vk::SampleCountFlagBits::e4;
		if (counts & vk::SampleCountFlagBits::e2)  return vk::SampleCountFlagBits::e2;

		return vk::SampleCountFlagBits::e1;
	}

	static const float priorities[] = { 1.0f };
	static vk::DeviceQueueCreateInfo createQueueCreateInfo(u32 index, u32 count = 1, const float* _priorities = priorities)
	{
		vk::DeviceQueueCreateInfo create_info;
		create_info.queueFamilyIndex = index;
		create_info.queueCount = count;
		create_info.pQueuePriorities = _priorities;
		return create_info;
	}

	static vk::Device createLogicalDevice(vk::PhysicalDevice physical_device, QueueFamilyIndices queue_indices, vk::Queue& queue)
	{
		Na::ArrayList<vk::DeviceQueueCreateInfo> queue_create_infos;
		queue_create_infos.emplace(createQueueCreateInfo(queue_indices.graphics));

		vk::DeviceCreateInfo create_info;

		create_info.queueCreateInfoCount = (u32)queue_create_infos.size();
		create_info.pQueueCreateInfos = queue_create_infos.ptr();

		vk::PhysicalDeviceFeatures device_features{};
		device_features.samplerAnisotropy = VK_TRUE;
		device_features.sampleRateShading = VK_TRUE;
		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = (u32)requiredDeviceExtensions.size();
		create_info.ppEnabledExtensionNames = requiredDeviceExtensions.ptr();

		vk::Device device = physical_device.createDevice(create_info);
		queue = device.getQueue(queue_indices.graphics, 0);

		return device;
	}

	static vk::CommandPool createSingleTimeCmdPool(vk::Device device, QueueFamilyIndices indices)
	{
		vk::CommandPoolCreateInfo single_time_pool_info;
		single_time_pool_info.queueFamilyIndex = indices.graphics;
		single_time_pool_info.flags = vk::CommandPoolCreateFlagBits::eTransient;

		return device.createCommandPool(single_time_pool_info);
	}

	VkContext::VkContext(initialize_t)
	{
		NA_VERIFY(!VkContext::s_Context, "Failed to create VkContext: Cannot create more than one Vulkan context!");
		VkContext::s_Context = this;

		NA_VERIFY(glfwVulkanSupported(), "Vulkan is not supported by GLFW!");

		g_Logger.print(Info, "Initializing Vulkan!");

		m_Instance = createInstance();
		m_DebugMessenger = createDebugMessenger(m_Instance);

		GLFWwindow* temp_window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
		vk::SurfaceKHR temp_surface = createWindowSurface(temp_window);

		m_PhysicalDevice = pickPhysicalDevice(m_Instance, temp_surface);
		auto queue_indices = QueueFamilyIndices::Get(m_PhysicalDevice, temp_surface);

		m_MSAASamples = getMaxSampleCount(m_PhysicalDevice);
		m_LogicalDevice = createLogicalDevice(m_PhysicalDevice, queue_indices, m_GraphicsQueue);
		m_SingleTimeCmdPool = createSingleTimeCmdPool(m_LogicalDevice, queue_indices);

		vkDestroySurfaceKHR(m_Instance, temp_surface, nullptr);
		glfwDestroyWindow(temp_window);
	}

	void VkContext::destroy(void)
	{
		if (!m_Valid)
			return;
		m_Valid = false;

		if (m_SingleTimeCmdPool)
			m_LogicalDevice.destroyCommandPool(m_SingleTimeCmdPool);

		if (m_LogicalDevice)
			m_LogicalDevice.destroy();

		if (m_DebugMessenger)
			DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

		if (m_Instance)
			m_Instance.destroy();

		VkContext::s_Context = nullptr;
	}

	vk::CommandBuffer VkContext::begin_single_time_cmds(void)
	{
		vk::CommandBufferAllocateInfo alloc_info;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandPool = m_SingleTimeCmdPool;
		alloc_info.commandBufferCount = 1;

		vk::CommandBuffer cmd_buffer;
		(void)m_LogicalDevice.allocateCommandBuffers(&alloc_info, &cmd_buffer);

		vk::CommandBufferBeginInfo begin_info;
		begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		cmd_buffer.begin(begin_info);

		return cmd_buffer;
	}

	void VkContext::end_single_time_cmds(vk::CommandBuffer cmd_buffer)
	{
		cmd_buffer.end();

		vk::SubmitInfo submit_info;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cmd_buffer;

		(void)m_GraphicsQueue.submit(1, &submit_info, nullptr);
		m_GraphicsQueue.waitIdle();

		m_LogicalDevice.freeCommandBuffers(s_Context->m_SingleTimeCmdPool, 1, &cmd_buffer);
	}

	VkContext::VkContext(VkContext&& other)
	: m_Instance(std::exchange(other.m_Instance, nullptr)),
	m_DebugMessenger(std::exchange(other.m_DebugMessenger, nullptr)),
	m_PhysicalDevice(std::exchange(other.m_PhysicalDevice, nullptr)),
	m_LogicalDevice(std::exchange(other.m_LogicalDevice, nullptr)),
	m_GraphicsQueue(std::exchange(other.m_GraphicsQueue, nullptr)),
	m_SingleTimeCmdPool(std::exchange(other.m_SingleTimeCmdPool, nullptr)),
	m_MSAASamples(other.m_MSAASamples),
	m_Valid(std::exchange(other.m_Valid, false))
	{
		VkContext::s_Context = this;
	}

	VkContext& VkContext::operator=(VkContext&& other)
	{
		m_Instance = std::exchange(other.m_Instance, nullptr);
		m_DebugMessenger = std::exchange(other.m_DebugMessenger, nullptr);
		m_PhysicalDevice = std::exchange(other.m_PhysicalDevice, nullptr);
		m_LogicalDevice = std::exchange(other.m_LogicalDevice, nullptr);
		m_GraphicsQueue = std::exchange(other.m_GraphicsQueue, nullptr);
		m_SingleTimeCmdPool = std::exchange(other.m_SingleTimeCmdPool, nullptr);
		m_MSAASamples = other.m_MSAASamples;
		m_Valid = std::exchange(other.m_Valid, false);

		VkContext::s_Context = this;
		return *this;
	}
} // namespace Na
