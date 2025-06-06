#include "Pch.hpp"
#include "Natrium/Assets/ShaderAsset.hpp"

#include "Natrium/Core/Logger.hpp"
#include "Natrium/Graphics/VkContext.hpp"

#include <shaderc/shaderc.hpp>

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	ArrayVector<u32> LoadSpv(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		NA_ASSERT(file, "Failed to open file {}", path.C_STR());

		u64 size = file.tellg();
		ArrayList<Byte> file_data(size + size % 4, size);

		file.seekg(0);
		file.read((char*)file_data.ptr(), size);
		file.close();

		while (file_data.size() % 4)
			file_data.emplace(0);

		ArrayVector<u32> spv((u32*)file_data.ptr(), file_data.size() / sizeof(u32));
		memset(&file_data, 0, sizeof(ArrayList<Byte>));
		return spv;
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

	ArrayVector<u32> ShaderString::compile(const std::string_view& entry_point) const
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
					g_Logger.fmt(
						Na::Warn,
						"Compiled {} with {} warnings!",
							m_Name,
							spv.GetNumWarnings()
					);
					g_Logger.log(Na::Warn, spv.GetErrorMessage());
				} else
				{
					g_Logger.fmt(
						Info,
						"Compiled {} with 0 warnings!",
							m_Name
					);
				}
			} else
			{
				g_Logger.fmt(
					Na::Error,
					"Failed to compile {} with {} errors and {} warnings!",
						m_Name,
						spv.GetNumErrors(),
						spv.GetNumWarnings()
				);
				g_Logger.log(Na::Error, spv.GetErrorMessage());
				throw std::runtime_error("Failed to compile shader!");
			}
		}

		return ArrayVector<u32>(spv.begin(), spv.end());
	}

	AssetHandle<ShaderBinary> ShaderBinary::Load(const std::filesystem::path& path)
	{
		return Ref<ShaderBinary>::Make(LoadSpv(path));
	}
} // namespace Na