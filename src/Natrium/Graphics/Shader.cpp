#include "Pch.hpp"
#include "Natrium/Graphics/Shader.hpp"

#include "Natrium/Assets/TextAsset.hpp"
#include "Natrium/Assets/AssetManager.hpp"

#include "Natrium/Graphics/Device.hpp"

#include "Natrium/Graphics/VulkanImpl/vShader.hpp"

namespace Na::Graphics {
	Expected<UniqueRef<Shader>, ShaderLoadingError> Shader::Make(
		const std::filesystem::path& path_to_glsl,
		ShaderStage stage,
		const std::string_view& entry_point
	) 
	{
		TextAsset glsl;

		FileErrorCode file_err_code = glsl.load(path_to_glsl);
		if (file_err_code != FileErrorCode::None)
		{
			return Unexpected(ShaderLoadingError(file_err_code));
		}

		const std::filesystem::path& input_path = path_to_glsl;
		std::filesystem::path output_path = AssetManager::Get()->shader_output_dir() / input_path.filename().replace_extension(".bin");

		g_Logger.print(Trace, "Creating shader: [");

		g_Logger.printf(Trace, "\tinput path: {}", input_path.string());
		g_Logger.printf(Trace, "\toutput path: {}", output_path.string());

		g_Logger.print(Trace, "]");

		bool should_compile = (
			!std::filesystem::exists(output_path) ||
			std::filesystem::last_write_time(input_path) > std::filesystem::last_write_time(output_path)
		);

		UniqueRef<Shader> shader;

		switch (Device::Get()->backend())
		{
			case DeviceBackend::Vulkan:
			{
				if (!should_compile)
				{
					if (auto expected = VulkanImpl::LoadSpirV(output_path))
					{
						shader = MakeUnique<VulkanImpl::Shader>(
							expected.value(),
							stage,
							entry_point
						);
					} else
					{
						return Unexpected(ShaderLoadingError(expected.error()));
					}
				} else
				{
					if (auto expected = VulkanImpl::CompileToSpirV(
						glsl.str(),
						input_path.filename().string(),
						entry_point
					))
					{
						const ArrayList<u32>& spv = expected.value();

						std::ofstream output_file(output_path, std::ios::binary);
						if (!output_file)
						{
							return Unexpected(ShaderLoadingError(FileErrorCode::Unknown));
						}

						output_file.write((const char*)spv.ptr(), spv.size() * sizeof(u32));
						output_file.close();

						shader = MakeUnique<VulkanImpl::Shader>(spv, stage, entry_point);
					} else
					{
						return Unexpected(ShaderLoadingError(expected.error()));
					}
				}
			}
		}

		return shader;
	}

	Shader::Shader(Shader&& other) noexcept
	: m_Stage(std::exchange(other.m_Stage, ShaderStage::None)),
	  m_EntryPoint(std::move(other.m_EntryPoint))
	{}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_Stage = std::exchange(other.m_Stage, ShaderStage::None);
		m_EntryPoint = std::move(other.m_EntryPoint);

		return *this;
	}
} // namespace Na::Graphics
