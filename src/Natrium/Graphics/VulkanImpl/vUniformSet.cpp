#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformSet.hpp"

#include "Natrium/Graphics/VulkanImpl/vUniformSetLayout.hpp"

#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"

#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vTexture.hpp"

#include "Internal.hpp"

namespace Na::VulkanImpl {
	UniformSet::UniformSet(
		View<const Graphics::UniformSetLayout> _layout,
		View<const Graphics::Renderer> _renderer
	)
	{
		auto layout = static_ref_cast<const UniformSetLayout>(_layout);
		auto renderer = static_ref_cast<const Renderer>(_renderer);

		m_Set = Internal::CreateDescriptorSet(layout->layout(), renderer->descriptor_pool());

		m_DynamicOffsetCount = layout->dynamic_count();
		m_DynamicOffsets.reallocate((u64)m_DynamicOffsetCount * renderer->settings()->max_frames_in_flight());
		m_DynamicOffsets.resize(m_DynamicOffsets.capacity());
	}

	void UniformSet::destroy(void)
	{
		m_DynamicOffsetIndex = 0;
		m_DynamicOffsetCount = 0;
		m_DynamicOffsets.destroy();
		m_Set = nullptr;
	}

	void UniformSet::bind_at(u32 binding, View<const Graphics::Texture> _texture)
	{
		auto texture = static_ref_cast<const Texture>(_texture);

		vk::DescriptorImageInfo image_info;
		image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		image_info.imageView = texture->img_view();
		image_info.sampler = texture->sampler();

		Internal::WriteToDescriptorSet(
			m_Set,
			binding,
			vk::DescriptorType::eCombinedImageSampler,
			1,
			nullptr, // buffer info
			&image_info,
			nullptr // texel buffer view
		);
	}

	void UniformSet::bind_at(u32 binding, View<const Graphics::Buffer> _buffer, BufferTypeFlags type)
	{
		auto buffer = static_ref_cast<const Buffer>(_buffer);

		vk::DescriptorType descriptor_type{};
		if (type == BufferTypeFlags::StorageBuffer)
		{
			descriptor_type = buffer->is_multibuffer() ? vk::DescriptorType::eStorageBufferDynamic : vk::DescriptorType::eStorageBuffer;
		} else
		if (type == BufferTypeFlags::UniformBuffer)
		{
			descriptor_type = buffer->is_multibuffer() ? vk::DescriptorType::eUniformBufferDynamic : vk::DescriptorType::eUniformBuffer;
		}


		vk::DescriptorBufferInfo buffer_info;

		buffer_info.buffer = buffer->native();
		buffer_info.offset = 0;
		buffer_info.range = buffer->aligned_size();

		Internal::WriteToDescriptorSet(
			m_Set,
			binding,
			descriptor_type,
			1, // count
			&buffer_info,
			nullptr, // image info
			nullptr // texel buffer view
		);

		if (buffer->is_multibuffer())
		{
			for (u64 i = m_DynamicOffsetIndex++; i < m_DynamicOffsets.size(); i += m_DynamicOffsetCount)
				m_DynamicOffsets[i] = (u32)(buffer->aligned_size() * i);
		}
	}
} // namespace Na::VulkanImpl
