#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vShader.hpp"

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

#include <shaderc/shaderc.hpp>

namespace Na::VulkanImpl {
	static ShaderErrorCode shadercStatusToNative(shaderc_compilation_status status)
	{
		switch (status)
		{

		case shaderc_compilation_status_success:           return ShaderErrorCode::None;
		case shaderc_compilation_status_invalid_stage:     return ShaderErrorCode::InvalidStage;
		case shaderc_compilation_status_compilation_error: return ShaderErrorCode::CompilationFailed;
		}
		return ShaderErrorCode::Unknown;
	}

	vk::ShaderStageFlagBits ShaderStageToVk(ShaderStage stage) {
		switch (stage)
		{
		case ShaderStage::Vertex:                 return vk::ShaderStageFlagBits::eVertex;
		case ShaderStage::Fragment:               return vk::ShaderStageFlagBits::eFragment;
		case ShaderStage::Compute:                return vk::ShaderStageFlagBits::eCompute;
		case ShaderStage::Geometry:               return vk::ShaderStageFlagBits::eGeometry;
		case ShaderStage::TessellationControl:    return vk::ShaderStageFlagBits::eTessellationControl;
		case ShaderStage::TessellationEvaluation: return vk::ShaderStageFlagBits::eTessellationEvaluation;

		default:                                  return vk::ShaderStageFlagBits(0);
		}
	}

	vk::Format VertexAttributeTypeToVk(VertexAttributeType type)
	{
		switch (type)
		{
		case VertexAttributeType::Float: return vk::Format::eR32Sfloat;
		case VertexAttributeType::Vec2:  return vk::Format::eR32G32Sfloat;
		case VertexAttributeType::Vec3:  return vk::Format::eR32G32B32Sfloat;
		case VertexAttributeType::Vec4:  return vk::Format::eR32G32B32A32Sfloat;

		default:                         return vk::Format::eUndefined;
		}
	}

	vk::DescriptorType UniformTypeToVk(UniformType type)
	{
		switch (type)
		{
		case UniformType::UniformBuffer: return vk::DescriptorType::eUniformBufferDynamic;
		case UniformType::StorageBuffer: return vk::DescriptorType::eStorageBufferDynamic;
		case UniformType::Texture:       return vk::DescriptorType::eCombinedImageSampler;

		default:                         return vk::DescriptorType(0);
		}
	}

	Expected<ArrayList<u32>, ShaderErrorCode> CompileToSpirV(
		const std::string_view& glsl,
		const std::string_view& name,
		const std::string_view& entry_point
	)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		if (k_BuildConfig != BuildConfig::Debug)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::SpvCompilationResult spv = compiler.CompileGlslToSpv(
			glsl.data(),
			glsl.size(),
			shaderc_glsl_infer_from_source,
			name.data(),
			entry_point.data(),
			options
		);

		if (g_Logger.enabled())
		{
			if (spv.GetCompilationStatus() == shaderc_compilation_status_success)
			{
				if (spv.GetNumWarnings())
				{
					g_Logger.printf(
						Na::Warn,
						"Compiled {} with {} warnings!",
						name,
						spv.GetNumWarnings()
					);
					g_Logger.print(Na::Warn, spv.GetErrorMessage());
				} else
				{
					g_Logger.printf(
						Info,
						"Compiled {} with 0 warnings!",
						name
					);
				}
			} else
			{
				g_Logger.printf(
					Na::Error,
					"Failed to compile {} with {} errors and {} warnings!",
					name,
					spv.GetNumErrors(),
					spv.GetNumWarnings()
				);
				g_Logger.print(Na::Error, spv.GetErrorMessage());

				return Unexpected(
					shadercStatusToNative(spv.GetCompilationStatus())
				);
			}
		}

		g_Logger.printf(Debug, "spv size: {}", std::distance(spv.begin(), spv.end()));

		return ArrayList<u32>(spv.begin(), spv.end());
	}

	Expected<ArrayList<u32>, FileErrorCode>LoadSpirV(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			return Unexpected(FileErrorCode::NotFound);
		}

		if (!std::filesystem::is_regular_file(path))
		{
			return Unexpected(FileErrorCode::InvalidFormat);
		}

		std::ifstream file(path, std::ios::ate | std::ios::binary);
		
		if (!file)
		{
			return Unexpected(FileErrorCode::Unknown);
		}

		u64 size = file.tellg();
		if (size % 4 != 0)
		{
			file.close();
			return Unexpected(FileErrorCode::InvalidFormat);
		}

		ArrayList<u32> file_data(size / 4);
		file_data.resize(file_data.capacity());

		file.seekg(0);
		file.read((char*)file_data.ptr(), size);
		file.close();

		return file_data;
	}

	Shader::Shader(const ArrayList<u32>& spv, ShaderStage stage, const std::string_view& entry_point)
	: Graphics::Shader(stage, entry_point)
	{
		NA_ASSERT(!spv.empty(), "Failed to create Shader: src was empty!");

		vk::ShaderModuleCreateInfo create_info;
		create_info.codeSize = spv.size() * sizeof(u32);
		create_info.pCode = spv.ptr();
		
		m_Module = Device::Get()->logical_device().createShaderModule(create_info);
	}

	void Shader::destroy(void) {
		if (!m_Module)
			return;

		Device::Get()->logical_device().destroyShaderModule(m_Module);
		m_Module = nullptr;
	}

	Shader::Shader(Shader&& other) noexcept
	: Graphics::Shader(std::move(other)),
	m_Module(std::exchange(other.m_Module, nullptr))
	{}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->destroy();
		Graphics::Shader::operator=(std::move(other));
		m_Module = std::exchange(other.m_Module, nullptr);

		return *this;
	}
} // namespace Na::VulkanImpl
