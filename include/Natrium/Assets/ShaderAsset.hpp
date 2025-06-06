#if !defined(NA_SHADER_ASSET_HPP)
#define NA_SHADER_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	enum class ShaderStageBits : u32 {
		None = 0,
		Vertex   = (u32)vk::ShaderStageFlagBits::eVertex,
		Fragment = (u32)vk::ShaderStageFlagBits::eFragment,

		All      = (u32)vk::ShaderStageFlagBits::eAll
	};

	class ShaderString : public Asset {
	public:
		ShaderString(const std::string& data, const std::string& name) : m_Data(data), m_Name(name) {};
		ShaderString(const std::string_view& data, const std::string_view& name) : m_Data(data), m_Name(name) {};
		ShaderString(const std::filesystem::path& path);

		static AssetHandle<ShaderString> Load(const std::filesystem::path& path) { return Ref<ShaderString>::Make(path);  }

		[[nodiscard]] ArrayVector<u32> compile(const std::string_view& entry_point = "main") const;

		[[nodiscard]] inline const std::string& data(void) const { return m_Data; }

		[[nodiscard]] inline operator bool(void) const override { return !m_Data.empty(); };
	private:
		std::string m_Data;
		std::string m_Name;
	};

	class ShaderBinary : public Asset {
	public:
		inline ShaderBinary(const ArrayVector<u32>& data) : m_Data(data) {}

		static AssetHandle<ShaderBinary> Load(const std::filesystem::path& path);

		[[nodiscard]] inline const ArrayVector<u32>& data(void) const { return m_Data; }
		[[nodiscard]] inline u64 size(void) const { return m_Data.size() * sizeof(u32); }
		[[nodiscard]] inline const u32* ptr(void) const { return m_Data.ptr(); }

		[[nodiscard]] inline operator bool(void) const override { return !m_Data.empty(); };
	private:
		ArrayVector<u32> m_Data;
	};
}

#endif // NA_SHADER_ASSET_HPP