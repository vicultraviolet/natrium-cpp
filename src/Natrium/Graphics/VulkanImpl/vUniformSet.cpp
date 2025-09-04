#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vUniformSet.hpp"

#include "Natrium/Graphics/VulkanImpl/vUniformSetLayout.hpp"

#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"

#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"
#include "Natrium/Graphics/VulkanImpl/vDeviceImage.hpp"
#include "Natrium/Graphics/VulkanImpl/vSampler.hpp"

#include "Internal.hpp"

namespace Na::VulkanImpl {
	static vk::DescriptorType bufferTypeToDescriptorType(BufferTypeFlags type, bool dynamic)
	{
		switch (type)
		{
		case BufferTypeFlags::UniformBuffer:
			return dynamic ? vk::DescriptorType::eUniformBufferDynamic : vk::DescriptorType::eUniformBuffer;
		case BufferTypeFlags::StorageBuffer:
			return dynamic ? vk::DescriptorType::eStorageBufferDynamic : vk::DescriptorType::eStorageBuffer;
		}
		return vk::DescriptorType(-1);
	}

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

	void UniformSet::bind(const UniformSetBufferBindingInfo& info)
	{
		auto buffer = static_ref_cast<const Buffer>(info.buffer);

		vk::DescriptorBufferInfo buffer_info;

		buffer_info.buffer = buffer->native();
		buffer_info.offset = 0;
		buffer_info.range = buffer->aligned_size();

		vk::DescriptorType descriptor_type = bufferTypeToDescriptorType(
			info.type, buffer->is_multibuffer()
		);

		Internal::WriteToDescriptorSet(
			m_Set,
			info.binding,
			descriptor_type,
			info.array_index,
			1, // count
			&buffer_info,
			nullptr, // image info
			nullptr // texel buffer view
		);

		if (buffer->is_multibuffer())
		{
			this->_set_dynamic_offsets_for_buffer(m_DynamicOffsetIndex++, (u32)buffer->aligned_size());
		}
	}

	void UniformSet::bind(const UniformSetTextureBindingInfo& info)
	{
		auto img = static_ref_cast<DeviceImage>(info.texture_info.img);
		auto sampler = static_ref_cast<Sampler>(info.texture_info.sampler);

		vk::DescriptorImageInfo image_info;
		image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		image_info.imageView = img->img_view();
		image_info.sampler = sampler->native();

		Internal::WriteToDescriptorSet(
			m_Set,
			info.binding,
			vk::DescriptorType::eCombinedImageSampler,
			info.array_index,
			1, // count
			nullptr, // buffer info
			&image_info,
			nullptr // texel buffer view
		);
	}

	void UniformSet::bind(const UniformSetStorageImageBindingInfo& info)
	{
		auto img = static_ref_cast<DeviceImage>(info.img);

		vk::DescriptorImageInfo image_info;
		image_info.imageLayout = vk::ImageLayout::eGeneral;
		image_info.imageView = img->img_view();

		Internal::WriteToDescriptorSet(
			m_Set,
			info.binding,
			vk::DescriptorType::eStorageImage,
			info.array_index,
			1, // count
			nullptr, // buffer info
			&image_info,
			nullptr // texel buffer view
		);
	}

	void UniformSet::bind_array(const UniformSetBufferBindingInfo2& info)
	{
		ArrayList<vk::DescriptorBufferInfo> buffer_infos(info.buffer_count, info.buffer_count);

		for (u32 i = 0; i < info.buffer_count; i++)
		{
			auto buffer = static_ref_cast<const Buffer>(info.buffers[i]);

			buffer_infos[i].buffer = buffer->native();
			buffer_infos[i].offset = 0;
			buffer_infos[i].range = buffer->aligned_size();
			
			if (buffer->is_multibuffer())
			{
				this->_set_dynamic_offsets_for_buffer(m_DynamicOffsetIndex++, (u32)buffer->aligned_size());
			}
		}

		bool is_dynamic = info.buffers[0]->is_multibuffer();

		vk::DescriptorType descriptor_type = bufferTypeToDescriptorType(info.type, is_dynamic);

		Internal::WriteToDescriptorSet(
			m_Set,
			info.binding,
			descriptor_type,
			info.starting_index,
			info.buffer_count, // count
			buffer_infos.ptr(),
			nullptr, // image info
			nullptr // texel buffer view
		);
	}

	void UniformSet::bind_array(const UniformSetTextureBindingInfo2& info)
	{
		ArrayList<vk::DescriptorImageInfo> img_infos(info.texture_count, info.texture_count);

		for (u32 i = 0; i < info.texture_count; i++)
		{
			auto img = static_ref_cast<DeviceImage>(info.texture_infos[i].img);
			auto sampler = static_ref_cast<Sampler>(info.texture_infos[i].sampler);

			img_infos[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			img_infos[i].imageView = img->img_view();
			img_infos[i].sampler = sampler->native();
		}

		Internal::WriteToDescriptorSet(
			m_Set,
			info.binding,
			vk::DescriptorType::eCombinedImageSampler,
			info.starting_index,
			info.texture_count,
			nullptr, // buffer info
			img_infos.ptr(),
			nullptr // texel buffer view
		);
	}

	void UniformSet::bind_array(const UniformSetStorageImageBindingInfo2& info)
	{
		ArrayList<vk::DescriptorImageInfo> img_infos(info.img_count, info.img_count);

		for (u32 i = 0; i < info.img_count; i++)
		{
			auto img = static_ref_cast<DeviceImage>(info.imgs[i]);

			img_infos[i].imageLayout = vk::ImageLayout::eGeneral;
			img_infos[i].imageView = img->img_view();
			img_infos[i].sampler = nullptr;
		}

		Internal::WriteToDescriptorSet(
			m_Set,
			info.binding,
			vk::DescriptorType::eCombinedImageSampler,
			info.starting_index,
			info.img_count,
			nullptr, // buffer info
			img_infos.ptr(),
			nullptr // texel buffer view
		);
	}

	void UniformSet::_set_dynamic_offsets_for_buffer(u32 dynamic_descriptor_index, u32 aligned_size)
	{
		u32 count = m_DynamicOffsets.size() / m_DynamicOffsetCount;
		for (u32 i = 0; i < count; i++)
		{
			u64 offset_index = (u64)i * (u64)m_DynamicOffsetCount + (u64)dynamic_descriptor_index;
			m_DynamicOffsets[offset_index] = i * aligned_size;
		}
	}
} // namespace Na::VulkanImpl
