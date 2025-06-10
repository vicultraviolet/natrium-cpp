#if !defined(NA_VK_CONTEXT_HPP)
#define NA_VK_CONTEXT_HPP

#include "Natrium/Core.hpp"

namespace Na {
    inline constexpr bool k_ValidationLayersEnabled = k_BuildConfig != BuildConfig::Distribution;

	struct SurfaceSupport {
		vk::SurfaceCapabilitiesKHR capabilities;
		Na::ArrayList<vk::SurfaceFormatKHR> formats;
		Na::ArrayList<vk::PresentModeKHR> present_modes;

		inline operator bool(void) const { return formats.size() && present_modes.size(); }

		static SurfaceSupport Get(vk::SurfaceKHR surface, vk::PhysicalDevice device);
	};

	class VkContext {
	public:
		VkContext(void) = default;

		VkContext(initialize_t);
		inline ~VkContext(void) { this->destroy(); }

		void destroy(void);

		VkContext(const VkContext& other) = delete;
		VkContext& operator=(const VkContext& other) = delete;

		VkContext(VkContext&& other);
		VkContext& operator=(VkContext&& other);

		inline void wait_for_device(void) { m_LogicalDevice.waitIdle(); }

		[[nodiscard]] vk::CommandBuffer begin_single_time_cmds(void);
		void end_single_time_cmds(vk::CommandBuffer cmd_buffer);

		[[nodiscard]] static inline bool Exists(void) { return VkContext::s_Context; }
		[[nodiscard]] static inline VkContext& Get(void) { return *VkContext::s_Context;  }

		[[nodiscard]] inline vk::Instance               instance(void)        const { return m_Instance; }
		[[nodiscard]] inline vk::DebugUtilsMessengerEXT debug_messenger(void) const { return m_DebugMessenger; }
		[[nodiscard]] inline vk::PhysicalDevice         physical_device(void) const { return m_PhysicalDevice; }
		[[nodiscard]] inline vk::Device                 logical_device(void)  const { return m_LogicalDevice; }

		[[nodiscard]] inline vk::Queue                  graphics_queue(void)  const { return m_GraphicsQueue; }

		[[nodiscard]] inline vk::SampleCountFlagBits    msaa_samples(bool enabled = true) const { return enabled ? m_MSAASamples : vk::SampleCountFlagBits::e1; }
	private:
		vk::Instance               m_Instance          = nullptr;
		vk::DebugUtilsMessengerEXT m_DebugMessenger    = nullptr;
		vk::PhysicalDevice         m_PhysicalDevice    = nullptr;
		vk::Device                 m_LogicalDevice     = nullptr;
												    
		vk::Queue                  m_GraphicsQueue     = nullptr;

		vk::CommandPool            m_SingleTimeCmdPool = nullptr;


		vk::SampleCountFlagBits    m_MSAASamples = vk::SampleCountFlagBits::e1;

		bool m_Valid = true;

		static inline VkContext* s_Context = nullptr;
	};
} // namespace Na

#endif // NA_VK_CONTEXT_HPP