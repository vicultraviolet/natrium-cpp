#if !defined(NA_INTERNAL_HPP)
#define NA_INTERNAL_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Na::Internal {
	struct DeviceLimits {
		vk::SampleCountFlagBits msaa_sample_count;
		float anisotropy;
	};

	struct DeviceData {
		vk::Instance instance = nullptr;
		vk::DebugUtilsMessengerEXT dbg_messenger = nullptr;
		vk::PhysicalDevice physical_device = nullptr;

		vk::Device logical_device = nullptr;

		vk::Queue graphics_queue = nullptr;
		u32 graphics_queue_index = k_U32Max;

		vk::CommandPool single_time_cmd_pool = nullptr;

		DeviceLimits limits{};

		[[nodiscard]] operator bool(void) const
		{
			return (
				instance &&
				physical_device &&
				logical_device &&
				graphics_queue &&
				graphics_queue_index != k_U32Max &&
				single_time_cmd_pool
			);
		}
	};
	inline DeviceData g_DeviceData{};

	vk::SurfaceKHR CreateWindowSurface(GLFWwindow* window);

	class QueueFamilyIndices {
	public:
		QueueFamilyIndices(void) = default;
		QueueFamilyIndices(vk::PhysicalDevice device, vk::SurfaceKHR surface);

		[[nodiscard]] inline u32 graphics(void) const { return m_Graphics; }

		[[nodiscard]] inline operator bool(void) const { return m_Graphics != k_U32Max; }
	private:
		u32 m_Graphics = k_U32Max;
	};

	class SurfaceSupport {
	public:
		SurfaceSupport(void) = default;
		SurfaceSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

		[[nodiscard]] inline vk::SurfaceCapabilitiesKHR capabilities(void) const { return m_Capabilities; }
		[[nodiscard]] inline const Na::ArrayList<vk::SurfaceFormatKHR>& formats(void) const { return m_Formats; }
		[[nodiscard]] inline const Na::ArrayList<vk::PresentModeKHR>& present_modes(void) { return m_PresentModes; }

		[[nodiscard]] inline operator bool(void) const { return m_Formats.size() && m_PresentModes.size(); }
	private:
		vk::SurfaceCapabilitiesKHR m_Capabilities;
		Na::ArrayList<vk::SurfaceFormatKHR> m_Formats;
		Na::ArrayList<vk::PresentModeKHR> m_PresentModes;
	};

	vk::CommandBuffer BeginSingleTimeCommands(void);
	void EndSingleTimeCommands(vk::CommandBuffer cmd_buffer);

	void WriteToDescriptorSet(
		vk::DescriptorSet set,
		u32 binding,
		vk::DescriptorType type,
		u32 count,
		vk::DescriptorBufferInfo* buffer_info = nullptr,
		vk::DescriptorImageInfo* image_info = nullptr,
		vk::BufferView* texel_buffer_view = nullptr
	);

	vk::Sampler CreateSampler(
		vk::Filter oversampling_filter,
		vk::Filter undersampling_filter,
		bool anisotropy_enabled,
		float max_anisotropy
	);
} // namespace Na::Internal

#endif // NA_RENDERER_INTERNAL_HPP