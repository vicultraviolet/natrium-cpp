#if !defined(NA_DEVICE_HPP)
#define NA_DEVICE_HPP

#include "Natrium/Core.hpp"

namespace Na::Graphics {
	enum class DeviceBackend : u8 {
		None = 0,
		// WebGPU,
		Vulkan
	};

	struct DeviceInitInfo {
		DeviceBackend backend;
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

		[[nodiscard]] virtual View<DeviceLimits> limits(void) { return nullptr; }
		[[nodiscard]] virtual View<const DeviceLimits> limits(void) const = 0;
	private:
		DeviceBackend m_Backend = DeviceBackend::None;

		static inline View<Device> s_Instance = nullptr;
	};
} // namespace Na

#endif // NA_DEVICE_HPP