#include "Pch.hpp"
#include "Internal.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

namespace Na::VulkanImpl {
	vk::SurfaceKHR Internal::CreateWindowSurface(GLFWwindow* window)
	{
		VkSurfaceKHR surface;
		VkResult result = glfwCreateWindowSurface(Device::Get()->instance(), window, nullptr, &surface);

		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface!");

		return surface;
	}

	Internal::QueueFamilyIndices::QueueFamilyIndices(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		auto properties = device.getQueueFamilyProperties();

		for (u32 i = 0; const auto& property : properties)
		{
			if (property.queueFlags & vk::QueueFlagBits::eGraphics)
				if (device.getSurfaceSupportKHR(i, surface))
					m_Graphics = i;

			if (*this)
				break;

			i++;
		}
	}

	Internal::SurfaceSupport::SurfaceSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		m_Capabilities = device.getSurfaceCapabilitiesKHR(surface);

		u32 format_count;
		(void)device.getSurfaceFormatsKHR(surface, &format_count, nullptr);
		m_Formats.reallocate(format_count, format_count);
		(void)device.getSurfaceFormatsKHR(surface, &format_count, m_Formats.ptr());

		u32 present_mode_count;
		(void)device.getSurfacePresentModesKHR(surface, &present_mode_count, nullptr);
		m_PresentModes.reallocate(present_mode_count, present_mode_count);
		(void)device.getSurfacePresentModesKHR(surface, &present_mode_count, m_PresentModes.ptr());
	}

	vk::CommandBuffer Internal::BeginSingleTimeCommands(void)
	{
		vk::CommandBufferAllocateInfo alloc_info;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandPool = Device::Get()->single_time_cmd_pool();
		alloc_info.commandBufferCount = 1;

		vk::CommandBuffer cmd_buffer;
		(void)Device::Get()->logical_device().allocateCommandBuffers(&alloc_info, &cmd_buffer);

		vk::CommandBufferBeginInfo begin_info;
		begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		cmd_buffer.begin(begin_info);

		return cmd_buffer;
	}

	void Internal::EndSingleTimeCommands(vk::CommandBuffer cmd_buffer)
	{
		cmd_buffer.end();

		vk::SubmitInfo submit_info;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cmd_buffer;

		(void)Device::Get()->graphics_queue().submit(1, &submit_info, nullptr);
		Device::Get()->graphics_queue().waitIdle();

		Device::Get()->logical_device().freeCommandBuffers(Device::Get()->single_time_cmd_pool(), 1, &cmd_buffer);
	}

	void Internal::WriteToDescriptorSet(
		vk::DescriptorSet set,
		u32 binding,
		vk::DescriptorType type,
		u32 count,
		vk::DescriptorBufferInfo* buffer_info, 
		vk::DescriptorImageInfo* image_info,
		vk::BufferView* texel_buffer_view
	)
	{
		vk::WriteDescriptorSet descriptor_write;
		descriptor_write.dstSet = set;
		descriptor_write.dstBinding = binding;
		descriptor_write.dstArrayElement = 0;

		descriptor_write.descriptorType = type;
		descriptor_write.descriptorCount = 1;

		descriptor_write.pBufferInfo = buffer_info;
		descriptor_write.pImageInfo = image_info;
		descriptor_write.pTexelBufferView = texel_buffer_view;

		Device::Get()->logical_device().updateDescriptorSets(
			1, &descriptor_write,
			0, nullptr // descriptor copy
		);
	}

	vk::Sampler Internal::CreateSampler(
		vk::Filter oversampling_filter,
		vk::Filter undersampling_filter,
		bool anisotropy_enabled,
		float max_anisotropy
	)
	{
		vk::SamplerCreateInfo create_info;

		create_info.magFilter = oversampling_filter;
		create_info.minFilter = undersampling_filter;

		create_info.addressModeU = vk::SamplerAddressMode::eRepeat;
		create_info.addressModeV = vk::SamplerAddressMode::eRepeat;
		create_info.addressModeW = vk::SamplerAddressMode::eRepeat;

		create_info.anisotropyEnable = anisotropy_enabled;
		create_info.maxAnisotropy = max_anisotropy;

		create_info.borderColor = vk::BorderColor::eIntOpaqueBlack;

		create_info.unnormalizedCoordinates = VK_FALSE;

		create_info.compareEnable = VK_FALSE;
		create_info.compareOp = vk::CompareOp::eAlways;

		create_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		create_info.mipLodBias = 0.0f;
		create_info.minLod = 0.0f;
		create_info.maxLod = 0.0f;

		return Device::Get()->logical_device().createSampler(create_info);
	}
} // namespace Na::VulkanImpl
