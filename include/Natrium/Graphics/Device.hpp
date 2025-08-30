#if !defined(NA_DEVICE_HPP)
#define NA_DEVICE_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::Graphics {
	enum class DeviceBackend : u8 {
		None = 0,
		// WebGPU,
		Vulkan
	};

	enum class DeviceExtension : u8 {
		None = 0,

		Swapchain,
		UniformIndexing,

		BindlessUniforms = UniformIndexing
	};
	using DeviceExtensions = std::set<DeviceExtension>;

	// info about what features to enable for bindless uniforms
	struct UniformIndexingInfo {
		// each type you want to use in a bindless way must be listed here
		std::set<UniformType> array_types;

		// each type you want to be able to update after binding must be listed here
		std::set<UniformType> update_after_bind_types;

		// support for updating unused uniforms in a set while the set is in use
		bool update_unused_while_in_use = false;

		// support for uniform sets where some uniforms in a binding may be invalid or unbound
		bool binding_partially_bound = false; 

		// support for setting the array count when creating the uniform set instead of the layout
		// without this, uniform arrays must have a fixed count that's set during layout creation
		// with this, you can specify the count only during the set creation
		// useful for bindless textures where the number of textures is not known at layout creation time
		// WARNING: this is only possible for the last binding in the layout!
		bool dynamic_count = false;

		// support for shaders to declare uniform arrays without a fixed shader compile-time size
		// like `sampler2D textures[];` instead of `sampler2D textures[16];`
		bool runtime_array = false;
	};

	struct DeviceInitInfo {
		DeviceBackend backend;
		DeviceExtensions required_extensions;
		std::optional<UniformIndexingInfo> uniform_indexing_info = std::nullopt;
	};
	
	enum class MSAASampleCount : u8 {
		None = 0,
		x1 = 1,
		x2 = 2,
		x4 = 4,
		x8 = 8,
		x16 = 16,
		x32 = 32,
		x64 = 64
	};

	class DeviceLimits {
	public:
		[[nodiscard]] virtual MSAASampleCount msaa_sample_count(void) const = 0;
		[[nodiscard]] virtual float max_anisotropy(void) const = 0;
	};

	class Device {
	public:
		[[nodiscard]] static UniqueRef<Device> Make(const DeviceInitInfo& info);

		Device(void) = default;
		Device(const DeviceInitInfo& info);

		virtual ~Device(void) { this->destroy(); }
		virtual void destroy(void);

		virtual void wait_all(void) const = 0;

		[[nodiscard]] virtual operator bool(void) const = 0;

		[[nodiscard]] static inline View<Device> Get(void) { return s_Instance; }

		[[nodiscard]] inline DeviceBackend backend(void) const { return m_Backend; }
		[[nodiscard]] inline const auto& extensions(void) const { return m_Extensions; }
		[[nodiscard]] inline const auto& uniform_indexing_info(void) const { return m_UniformIndexingInfo; }

		[[nodiscard]] virtual View<DeviceLimits> limits(void) { return nullptr; }
		[[nodiscard]] virtual View<const DeviceLimits> limits(void) const = 0;
	private:
		DeviceBackend m_Backend = DeviceBackend::None;
		DeviceExtensions m_Extensions;
		UniformIndexingInfo m_UniformIndexingInfo;

		static inline View<Device> s_Instance = nullptr;
	};
} // namespace Na

#endif // NA_DEVICE_HPP