#if !defined(NA_VULKAN_IMPL_UNIFORM_SET_HPP)
#define NA_VULKAN_IMPL_UNIFORM_SET_HPP

#include "Natrium/Graphics/UniformSet.hpp"
#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"

namespace Na::VulkanImpl {
	using UniformSetBufferBindingInfo        = Graphics::UniformSetBufferBindingInfo;
	using UniformSetTextureBindingInfo       = Graphics::UniformSetTextureBindingInfo;
	using UniformSetStorageImageBindingInfo  = Graphics::UniformSetStorageImageBindingInfo;

	using UniformSetBufferBindingInfo2       = Graphics::UniformSetBufferBindingInfo2;
	using UniformSetTextureBindingInfo2      = Graphics::UniformSetTextureBindingInfo2;
	using UniformSetStorageImageBindingInfo2 = Graphics::UniformSetStorageImageBindingInfo2;

	class UniformSet : public Graphics::UniformSet {
	public:
		UniformSet(void) = default;
		UniformSet(View<const Graphics::UniformSetLayout> layout, View<const Graphics::Renderer> renderer);

		~UniformSet(void) { this->destroy(); }
		void destroy(void);

		void bind(const UniformSetBufferBindingInfo& info) override;
		void bind(const UniformSetTextureBindingInfo& info) override;
		void bind(const UniformSetStorageImageBindingInfo& info) override;

		void bind_array(const UniformSetBufferBindingInfo2& info) override;
		void bind_array(const UniformSetTextureBindingInfo2& info) override;
		void bind_array(const UniformSetStorageImageBindingInfo2& info) override;

		[[nodiscard]] inline vk::DescriptorSet& descriptor_set(void) { return m_Set; }
		[[nodiscard]] inline const vk::DescriptorSet& descriptor_set(void) const { return m_Set; }

		[[nodiscard]] inline const ArrayList<u32>& dynamic_offsets(void) const { return m_DynamicOffsets; }
		[[nodiscard]] inline u32 dynamic_offset_count(void) const { return m_DynamicOffsetCount; }

		[[nodiscard]] inline operator bool(void) const override { return m_Set; }
	private:
		void _set_dynamic_offsets_for_buffer(u32 dynamic_descriptor_index, u32 aligned_size);
	private:
		vk::DescriptorSet m_Set = nullptr;

		ArrayList<u32> m_DynamicOffsets;
		u32 m_DynamicOffsetCount = 0;
		u32 m_DynamicOffsetIndex = 0;
	};
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_UNIFORM_SET_HPP