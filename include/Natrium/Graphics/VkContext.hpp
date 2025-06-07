#if !defined(NA_VK_CONTEXT_HPP)
#define NA_VK_CONTEXT_HPP

#include "Natrium/Core.hpp"

namespace Na {
    inline constexpr bool k_ValidationLayersEnabled = k_BuildConfig != BuildConfig::Distribution;

	struct SurfaceSupport {
		vk::SurfaceCapabilitiesKHR capabilities;
		Na::ArrayVector<vk::SurfaceFormatKHR> formats;
		Na::ArrayVector<vk::PresentModeKHR> present_modes;

		inline operator bool(void) const { return formats.size() && present_modes.size(); }

		static SurfaceSupport Get(vk::SurfaceKHR surface, vk::PhysicalDevice device);
	};

	class VkContext {
	public:
		VkContext(void) = default;

		VkContext(const VkContext& other) = delete;
		VkContext& operator=(const VkContext& other) = delete;

		VkContext(VkContext&& other);
		VkContext& operator=(VkContext&& other);

		static VkContext Initialize(void);
		static void Shutdown(void);

		static inline void WaitForRemainingDeviceTasks(void) { s_Context->m_LogicalDevice.waitIdle(); }

		static vk::CommandBuffer BeginSingleTimeCommands(void);
		static void EndSingleTimeCommands(vk::CommandBuffer cmd_buffer);

		[[nodiscard]] static inline bool Exists(void) { return VkContext::s_Context;  }

		[[nodiscard]] static inline vk::Instance               GetInstance(void)       { return s_Context->m_Instance; }
		[[nodiscard]] static inline vk::DebugUtilsMessengerEXT GetDebugMessenger(void) { return s_Context->m_DebugMessenger; }
		[[nodiscard]] static inline vk::PhysicalDevice         GetPhysicalDevice(void) { return s_Context->m_PhysicalDevice; }
		[[nodiscard]] static inline vk::Device                 GetLogicalDevice(void)  { return s_Context->m_LogicalDevice; }

		[[nodiscard]] static inline vk::Queue                  GetGraphicsQueue(void)  { return s_Context->m_GraphicsQueue; }


		[[nodiscard]] static inline vk::SampleCountFlagBits    GetMSAASamples(bool enabled = true) { return enabled ? s_Context->m_MSAASamples : vk::SampleCountFlagBits::e1; }

	private:
		vk::Instance               m_Instance;
		vk::DebugUtilsMessengerEXT m_DebugMessenger;
		vk::PhysicalDevice         m_PhysicalDevice;
		vk::Device                 m_LogicalDevice;
												    
		vk::Queue                  m_GraphicsQueue;

		vk::CommandPool            m_SingleTimeCmdPool;


		vk::SampleCountFlagBits    m_MSAASamples = vk::SampleCountFlagBits::e1;

		static inline VkContext* s_Context = nullptr;
	};
} // namespace Na

#endif // NA_VK_CONTEXT_HPP