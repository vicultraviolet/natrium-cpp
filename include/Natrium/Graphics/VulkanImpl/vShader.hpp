#if !defined(NA_VULKAN_IMPL_SHADER_HPP)
#define NA_VULKAN_IMPL_SHADER_HPP

#include "Natrium/Graphics/VertexAttributes.hpp"
#include "Natrium/Graphics/Shader.hpp"

namespace Na::VulkanImpl {
	using ShaderStage = Na::Graphics::ShaderStage;
	using VertexAttributeType = Graphics::VertexAttributeType;
	using UniformType = Graphics::UniformType;

	vk::ShaderStageFlagBits ShaderStageToVk(ShaderStage stage);
	vk::Format VertexAttributeTypeToVk(VertexAttributeType type);
	vk::DescriptorType UniformTypeToVk(UniformType type);

	ArrayList<u32> CompileToSpirV(
		const std::string_view& glsl,
		const std::string_view& name,
		const std::string_view& entry_point = "main"
	);

	ArrayList<u32> LoadSpirV(const std::filesystem::path& path);

	class Shader : public Graphics::Shader {
	public:
		Shader(void) = default;
		Shader(const ArrayList<u32>& spv, ShaderStage stage, const std::string_view& entry_point = "main");

		~Shader(void) { this->destroy(); }
		void destroy(void) override;
		
		Shader(const Shader& other) = delete;
		Shader& operator=(const Shader& other) = delete;

		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		[[nodiscard]] inline vk::ShaderModule& module(void) { return m_Module; }
		[[nodiscard]] inline vk::ShaderModule module(void) const { return m_Module; }
	private:
		vk::ShaderModule m_Module = nullptr;
	};
} // namespace Na::VulkanImpl

#endif // NA_VULKAN_IMPL_SHADER_HPP