#include "Pch.hpp"
#include "Natrium/Assets/ShaderAsset.hpp"

#include <shaderc/shaderc.hpp>

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	ArrayList<u32> LoadSpv(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		NA_ASSERT(file, "Failed to open file {}", path.C_STR());

		u64 size = file.tellg();
		NA_ASSERT(size % 4 == 0, "Failed to Load SPIR-V: {} File size wasn't a multiple of 4!", path.C_STR());

		ArrayList<u32> file_data(size / 4);
		file_data.resize(file_data.capacity());

		file.seekg(0);
		file.read((char*)file_data.ptr(), size);
		file.close();

		return file_data;
	}

	ShaderString::ShaderString(const std::filesystem::path& path)
	: m_Name(path.filename().string())
	{
		std::ifstream shader_file(path, std::ios::ate | std::ios::binary);
		NA_ASSERT(shader_file, "Failed to open file {}", path.C_STR());

		m_Data.resize((u64)shader_file.tellg());

		shader_file.seekg(0);
		shader_file.read(m_Data.data(), m_Data.size());
		shader_file.close();
	}

	ArrayList<u32> ShaderString::compile(const std::string_view& entry_point) const
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		if (k_BuildConfig != BuildConfig::Debug)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::SpvCompilationResult spv = compiler.CompileGlslToSpv(
			m_Data.c_str(),
			m_Data.size(),
			shaderc_glsl_infer_from_source,
			m_Name.data(),
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
							m_Name,
							spv.GetNumWarnings()
					);
					g_Logger.print(Na::Warn, spv.GetErrorMessage());
				} else
				{
					g_Logger.printf(
						Info,
						"Compiled {} with 0 warnings!",
							m_Name
					);
				}
			} else
			{
				g_Logger.printf(
					Na::Error,
					"Failed to compile {} with {} errors and {} warnings!",
						m_Name,
						spv.GetNumErrors(),
						spv.GetNumWarnings()
				);
				g_Logger.print(Na::Error, spv.GetErrorMessage());
				throw std::runtime_error("Failed to compile shader!");
			}
		}

		g_Logger.printf(Debug, "spv size: {}", std::distance(spv.begin(), spv.end()));

		return ArrayList<u32>(spv.begin(), spv.end());
	}

	AssetHandle<ShaderBinary> ShaderBinary::Load(const std::filesystem::path& path)
	{
		return Ref<ShaderBinary>::Make(LoadSpv(path));
	}
} // namespace Na