#if !defined(NA_VULKAN_IMPL_DEVICE_HPP)
#define NA_VULKAN_IMPL_DEVICE_HPP

#include "Natrium/Graphics/Device.hpp"

namespace Na::VulkanImpl {
	inline constexpr bool k_ValidationLayersEnabled = k_BuildConfig != BuildConfig::Distribution;

	using DeviceExtension = Graphics::DeviceExtension;
	const char* DeviceExtensionToVk(DeviceExtension extension);

	using DeviceExtensions = Graphics::DeviceExtensions;
	ArrayList<const char*> DeviceExtensionsToVk(const DeviceExtensions& extensions);

	using UniformIndexingInfo = Graphics::UniformIndexingInfo;
	vk::PhysicalDeviceDescriptorIndexingFeaturesEXT
		UniformIndexingInfoToVk(const UniformIndexingInfo& info);

	using DeviceInitInfo = Graphics::DeviceInitInfo;
	using DeviceBackend = Graphics::DeviceBackend;

	using MSAASampleCount = Graphics::MSAASampleCount;

	class DeviceLimits : public Graphics::DeviceLimits {
	public:
		vk::SampleCountFlagBits vk_msaa_sample_count{0};
		float vk_max_anisotropy = 0.0f;

		[[nodiscard]] vk::SampleCountFlagBits vk_msaa_sample_count_if(bool enabled) const;

		[[nodiscard]] MSAASampleCount msaa_sample_count(void) const override;
		[[nodiscard]] float max_anisotropy(void) const override;
	};

	class Device : public Graphics::Device {
	public:
		Device(void) = default;
		Device(const DeviceInitInfo& info);

		~Device(void) { this->destroy(); }
		void destroy(void) override;

		inline void wait_all(void) const override { m_LogicalDevice.waitIdle(); }

		[[nodiscard]] inline operator bool(void) const override { return m_Initialized; }

		[[nodiscard]] static inline View<Device> Get(void) { return static_ref_cast<Device>(Graphics::Device::Get()); }

		[[nodiscard]] inline vk::Instance& instance(void) { return m_Instance; }
		[[nodiscard]] inline const vk::Instance& instance(void) const { return m_Instance; }

		[[nodiscard]] inline vk::DebugUtilsMessengerEXT& debug_messenger(void) { return m_DebugMessenger; }
		[[nodiscard]] inline const vk::DebugUtilsMessengerEXT& debug_messenger(void) const { return m_DebugMessenger; }

		[[nodiscard]] inline vk::PhysicalDevice& physical_device(void) { return m_PhysicalDevice; }
		[[nodiscard]] inline const vk::PhysicalDevice& physical_device(void) const { return m_PhysicalDevice; }

		[[nodiscard]] inline vk::Device& logical_device(void) { return m_LogicalDevice; }
		[[nodiscard]] inline const vk::Device& logical_device(void) const { return m_LogicalDevice; }

		[[nodiscard]] inline vk::Queue& graphics_queue(void) { return m_GraphicsQueue; }
		[[nodiscard]] inline const vk::Queue& graphics_queue(void) const { return m_GraphicsQueue; }

		[[nodiscard]] inline u32 graphics_queue_index(void) const { return m_GraphicsQueueIndex; }

		[[nodiscard]] inline vk::CommandPool& single_time_cmd_pool(void) { return m_SingleTimeCommandPool; }
		[[nodiscard]] inline const vk::CommandPool& single_time_cmd_pool(void) const { return m_SingleTimeCommandPool; }

		[[nodiscard]] inline View<Graphics::DeviceLimits> limits(void) override { return &m_Limits; }
		[[nodiscard]] inline View<const Graphics::DeviceLimits> limits(void) const override { return &m_Limits; }

		[[nodiscard]] inline const DeviceLimits& vk_limits(void) const { return m_Limits; }
	private:
		void _create_instance(void);
		void _create_dbg_messenger(void);
		void _pick_physical_device(vk::SurfaceKHR surface, const Na::ArrayList<const char*>& extensions);
		void _get_limits(void);
		void _create_logical_device(
			vk::SurfaceKHR surface,
			const Na::ArrayList<const char*>& extensions,
			vk::PhysicalDeviceDescriptorIndexingFeaturesEXT* descriptor_indexing_features 
		);
		void _create_single_time_cmd_pool(void);
	private:
		vk::Instance m_Instance = nullptr;
		vk::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;

		vk::PhysicalDevice m_PhysicalDevice = nullptr;
		DeviceLimits m_Limits{};

		vk::Device m_LogicalDevice = nullptr;

		vk::Queue m_GraphicsQueue = nullptr;
		u32 m_GraphicsQueueIndex = u32max;

		bool m_Initialized = false;

		vk::CommandPool m_SingleTimeCommandPool = nullptr;
	};
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_DEVICE_HPP