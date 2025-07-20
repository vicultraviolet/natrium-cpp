#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformSet.hpp"

#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"

#include "Natrium/Graphics/VulkanImpl/vUniformBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vStorageBuffer.hpp"
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

	void UniformSet::bind_at(u32 binding, View<const Graphics::Uniform> uniform)
	{
		UniformType type = uniform->type();
		switch (type)
		{
		case UniformType::Texture:
		{
			auto texture = static_ref_cast<const Texture>(uniform);

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
			break;
		}
		case UniformType::UniformBuffer:
		{
			auto ubo = static_ref_cast<const UniformBuffer>(uniform);

			vk::DescriptorBufferInfo buffer_info(ubo->buffer().buffer, 0, ubo->aligned_size());

			Internal::WriteToDescriptorSet(
				m_Set,
				binding,
				vk::DescriptorType::eUniformBufferDynamic,
				1,
				&buffer_info,
				nullptr, // image info
				nullptr // texel buffer view
			);

			for (u64 i = m_DynamicOffsetIndex++; i < m_DynamicOffsets.size(); i += m_DynamicOffsetCount)
				m_DynamicOffsets[i] = (u32)(ubo->aligned_size() * i);

			break;
		}
		case UniformType::StorageBuffer:
		{
			auto ssbo = static_ref_cast<const StorageBuffer>(uniform);

			vk::DescriptorBufferInfo buffer_info(ssbo->buffer().buffer, 0, ssbo->aligned_size());

			Internal::WriteToDescriptorSet(
				m_Set,
				binding,
				vk::DescriptorType::eStorageBufferDynamic,
				1,
				&buffer_info,
				nullptr, // image info
				nullptr // texel buffer view
			);

			for (u64 i = m_DynamicOffsetIndex++; i < m_DynamicOffsets.size(); i += m_DynamicOffsetCount)
				m_DynamicOffsets[i] = (u32)(ssbo->aligned_size() * i);

			break;
		}
		default:
			throw std::runtime_error("Failed to bind uniform to pipeline: Uniform object of unknown type!");
		}
	}
} // namespace Na::VulkanImpl
