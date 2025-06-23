#if !defined(NA_SHADER_ASSET_HPP)
#define NA_SHADER_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	enum class ShaderStage : u8 {
		None = 0,
		Vertex, Geometry, Fragment,
		Compute
	};

	class ShaderString : public Asset {
	public:
		ShaderString(const std::string& data, const std::string& name)
		: m_Data(data), m_Name(name)
		{}

		ShaderString(const std::string_view& data, const std::string_view& name)
		: m_Data(data), m_Name(name)
		{}

		ShaderString(const std::filesystem::path& path);

		static AssetHandle<ShaderString> Load(const std::filesystem::path& path) { return Ref<ShaderString>::Make(path);  }

		[[nodiscard]] ArrayList<u32> compile(const std::string_view& entry_point = "main") const;

		[[nodiscard]] inline const std::string& data(void) const { return m_Data; }

		[[nodiscard]] inline operator bool(void) const override { return !m_Data.empty(); };
	private:
		std::string m_Data;
		std::string m_Name;
	};

	class ShaderBinary : public Asset {
	public:
		inline ShaderBinary(const ArrayList<u32>& data) : m_Data(data) {}

		static AssetHandle<ShaderBinary> Load(const std::filesystem::path& path);

		[[nodiscard]] inline const ArrayList<u32>& data(void) const { return m_Data; }
		[[nodiscard]] inline u64 size(void) const { return m_Data.size(); }
		[[nodiscard]] inline const u32* ptr(void) const { return (u32*)m_Data.ptr(); }

		[[nodiscard]] inline operator bool(void) const override { return !m_Data.empty(); };
	private:
		ArrayList<u32> m_Data;
	};
}

#endif // NA_SHADER_ASSET_HPP