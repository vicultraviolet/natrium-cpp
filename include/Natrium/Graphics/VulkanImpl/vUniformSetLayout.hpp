#if !defined(NA_VULKAN_IMPL_UNIFORM_SET_LAYOUT_HPP)
#define NA_VULKAN_IMPL_UNIFORM_SET_LAYOUT_HPP

#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::VulkanImpl {
	using UniformBinding = Graphics::UniformBinding;
	using UniformType = Graphics::UniformType;

	vk::DescriptorType UniformTypeToVk(UniformType type);

	class UniformSetLayout : public Graphics::UniformSetLayout {
	public:
		UniformSetLayout(void) = default;
		UniformSetLayout(const UniformBinding* bindings, u64 binding_count);

		void destroy(void) override;
		~UniformSetLayout(void) { this->destroy(); }

		[[nodiscard]] vk::DescriptorSetLayout& layout(void) { return m_Layout; }
		[[nodiscard]] const vk::DescriptorSetLayout& layout(void) const { return m_Layout; }

		[[nodiscard]] u32 dynamic_count(void) const { return m_DynamicCount; }

		[[nodiscard]] inline operator bool(void) const override { return m_Layout; }
	private:
		vk::DescriptorSetLayout m_Layout = nullptr;
		u32 m_DynamicCount = 0;
	};
	using DescriptorSetLayout = UniformSetLayout;
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_UNIFORM_SET_LAYOUT_HPP