#if !defined(NA_VULKAN_IMPL_SAMPLER_HPP)
#define NA_VULKAN_IMPL_SAMPLER_HPP

#include "Natrium/Graphics/Sampler.hpp"

namespace Na::VulkanImpl {
	using SamplerFilter = Graphics::SamplerFilter;
	using SamplerAddressMode = Graphics::SamplerAddressMode;
	using SamplerCreateInfo = Graphics::SamplerCreateInfo;

	vk::Filter SamplerFilterToVk(SamplerFilter filter);
	vk::SamplerAddressMode SamplerAddressModeToVk(SamplerAddressMode mode);

	class Sampler : public Graphics::Sampler {
	public:
		Sampler(const SamplerCreateInfo& info);
		~Sampler(void);

		[[nodiscard]] inline vk::Sampler& native(void) { return m_Sampler; }
		[[nodiscard]] inline const vk::Sampler& native(void) const { return m_Sampler; }

		[[nodiscard]] inline operator bool(void) const override { return m_Sampler; }
	private:
		vk::Sampler m_Sampler = nullptr;
	};
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_SAMPLER_HPP