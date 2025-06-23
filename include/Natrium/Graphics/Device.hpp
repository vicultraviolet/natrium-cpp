#if !defined(NA_DEVICE_HPP)
#define NA_DEVICE_HPP

#include "Natrium/Core.hpp"

namespace Na {
	inline constexpr bool k_ValidationLayersEnabled = k_BuildConfig != BuildConfig::Distribution;

	enum class RendererAPI : u8 {
		None = 0,
		// WebGPU,
		Vulkan
	};

	struct DeviceInitInfo {
		RendererAPI api;
	};

	class DeviceLimits {
	public:
		[[nodiscard]] static vk::SampleCountFlagBits MSAASampleCount(void);
		[[nodiscard]] static inline vk::SampleCountFlagBits MSAASampleCount(bool enabled) { return enabled ? DeviceLimits::MSAASampleCount() : vk::SampleCountFlagBits::e1; }
		[[nodiscard]] static float Anisotropy(void);
	};

	class Device {
	public:
		using Limits = DeviceLimits;

		static void Initialize(const DeviceInitInfo& info = DeviceInitInfo{});
		static void Shutdown(void);

		Device(void) = default;
		~Device(void) { if (m_Valid) Device::Shutdown(); }

		Device(const DeviceInitInfo& info) : m_Valid(true) { Device::Initialize(info); }

		Device(const Device& other) = delete;
		Device& operator=(const Device& other) = delete;

		Device(Device&& other) = default;
		Device& operator=(Device&& other) = default;

		[[nodiscard]] static inline Device Get(void) { return Device(); }

		void wait_all(void) const;

		[[nodiscard]] bool initialized(void) const;
		[[nodiscard]] RendererAPI api(void) const;
	private:
		static void _CreateInstance(void);
		static void _CreateDebugMessenger(void);
		static void _PickPhysicalDevice(vk::SurfaceKHR surface, const Na::ArrayList<const char*>& extensions);
		static void _CreateLogicalDevice(vk::SurfaceKHR surface, const Na::ArrayList<const char*>& extensions);
		static void _CreateSingleTimeCommandPool(void);
		static void _GetLimits(void);
	private:
		bool m_Valid = false;
	};
} // namespace Na

#endif // NA_DEVICE_HPP