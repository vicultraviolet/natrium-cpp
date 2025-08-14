#if !defined(NA_VULKAN_IMPL_UNIFORM_SET_HPP)
#define NA_VULKAN_IMPL_UNIFORM_SET_HPP

#include "Natrium/Graphics/UniformSet.hpp"
#include "Natrium/Graphics/VulkanImpl/vBuffer.hpp"

namespace Na::VulkanImpl {
	class UniformSet : public Graphics::UniformSet {
	public:
		UniformSet(void) = default;
		UniformSet(View<const Graphics::UniformSetLayout> layout, View<const Graphics::Renderer> renderer);

		~UniformSet(void) { this->destroy(); }
		void destroy(void);

		// type should be EITHER StorageBuffer OR UniformBuffer
		void bind_at(u32 binding, View<const Graphics::Buffer> buffer, BufferTypeFlags type) override;
		void bind_at(u32 binding, View<const Graphics::Texture> texture) override;
		
		[[nodiscard]] inline vk::DescriptorSet& descriptor_set(void) { return m_Set; }
		[[nodiscard]] inline const vk::DescriptorSet& descriptor_set(void) const { return m_Set; }

		[[nodiscard]] inline const ArrayList<u32>& dynamic_offsets(void) const { return m_DynamicOffsets; }
		[[nodiscard]] inline u32 dynamic_offset_count(void) const { return m_DynamicOffsetCount; }

		[[nodiscard]] inline operator bool(void) const override { return m_Set; }
	private:
		vk::DescriptorSet m_Set = nullptr;

		ArrayList<u32> m_DynamicOffsets;
		u32 m_DynamicOffsetCount = 0;
		u32 m_DynamicOffsetIndex = 0;
	};
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_UNIFORM_SET_HPP