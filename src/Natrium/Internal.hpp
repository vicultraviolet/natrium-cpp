#if !defined(NA_VULKAN_IMPL_INTERNAL_HPP)
#define NA_VULKAN_IMPL_INTERNAL_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Na::VulkanImpl::Internal {
	vk::SurfaceKHR CreateWindowSurface(GLFWwindow* window);

	class QueueFamilyIndices {
	public:
		QueueFamilyIndices(void) = default;
		QueueFamilyIndices(vk::PhysicalDevice device, vk::SurfaceKHR surface);

		[[nodiscard]] inline u32 graphics(void) const { return m_Graphics; }

		[[nodiscard]] inline operator bool(void) const { return m_Graphics != u32max; }
	private:
		u32 m_Graphics = u32max;
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
} // namespace Na::VulkanImpl::Internal

#endif // NA_VULKAN_IMPL_INTERNAL_HPP