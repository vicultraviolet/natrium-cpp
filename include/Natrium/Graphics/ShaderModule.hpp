#if !defined(NA_SHADER_MODULE_HPP)
#define NA_SHADER_MODULE_HPP

#include "Natrium/Assets/ShaderAsset.hpp"

namespace Na {
	class ShaderModule {
	public:
		inline ShaderModule(
			vk::ShaderModule module = nullptr,
			ShaderStage stage = ShaderStage::None,
			const std::string_view& entry_point = "main"
		)
		: m_Module(module), m_Stage(stage), m_EntryPoint(entry_point)
		{}

		ShaderModule(
			const ShaderBinary& binary,
			ShaderStage stage,
			const std::string_view& entry_point = "main"
		);

		~ShaderModule(void);

		ShaderModule(ShaderModule&& other);
		ShaderModule& operator=(ShaderModule&& other);

		[[nodiscard]] vk::PipelineShaderStageCreateInfo pipeline_shader_info(void) const;

		[[nodiscard]] inline vk::ShaderModule module(void) const { return m_Module; }
		[[nodiscard]] inline ShaderStage stage(void) const { return m_Stage; }

		[[nodiscard]] inline std::string_view& entry_point(void) { return m_EntryPoint; }
		[[nodiscard]] inline const std::string_view& entry_point(void) const { return m_EntryPoint; }

		[[nodiscard]] inline operator bool(void) const { return m_Module; };
	private:
		vk::ShaderModule m_Module;
		ShaderStage m_Stage;
		std::string_view m_EntryPoint;
	};
}

#endif // NA_SHADER_MODULE_HPP