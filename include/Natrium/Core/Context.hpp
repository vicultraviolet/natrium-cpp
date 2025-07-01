#if !defined(NA_CONTEXT_HPP)
#define NA_CONTEXT_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Core/Event.hpp"

namespace Na {
	struct ContextInitInfo {
	};

	class Context {
	public:
		Context(void) = default;

		explicit Context(const ContextInitInfo& info);
		~Context(void) { this->destroy(); }

		void destroy(void);

		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

		Context(Context&& other);
		Context& operator=(Context&& other);

		[[nodiscard]] static inline bool Exists(void) { return Context::s_Context; }
		[[nodiscard]] static inline Context& Get(void) { return *Context::s_Context; }

		[[nodiscard]] inline EventQueue& event_queue(void) { return m_EventQueue; }
		[[nodiscard]] inline const EventQueue& event_queue(void) const { return m_EventQueue; }

		[[nodiscard]] inline const std::filesystem::path& exec_path(void) const { return m_ExecPath; }
		[[nodiscard]] inline const std::filesystem::path& exec_dir(void)  const { return m_ExecDir; }
		[[nodiscard]] inline const std::filesystem::path& exec_name(void) const { return m_ExecName; }

		[[nodiscard]] inline std::random_device& random_device(void) { return m_RandomDevice; }
		[[nodiscard]] inline const std::random_device& random_device(void) const { return m_RandomDevice; }

		[[nodiscard]] inline std::mt19937& random_generator(void) { return m_RandomGenerator; }
		[[nodiscard]] inline const std::mt19937& random_generator(void) const { return m_RandomGenerator; }

		[[nodiscard]] inline auto& uuid_generator(void) { return m_UUIDGenerator; }
		[[nodiscard]] inline const auto& uuid_generator(void) const { return m_UUIDGenerator; }

		[[nodiscard]] inline auto& uuid_name_generator(void) { return m_UUIDNameGenerator; }
		[[nodiscard]] inline const auto& uuid_name_generator(void) const { return m_UUIDNameGenerator; }
		
	private:
		std::filesystem::path m_ExecPath, m_ExecDir, m_ExecName;
		std::string_view m_Version;

		EventQueue m_EventQueue;

		bool m_Valid = true;

		std::random_device m_RandomDevice;
		std::mt19937 m_RandomGenerator{ m_RandomDevice() };
		uuids::basic_uuid_random_generator<std::mt19937> m_UUIDGenerator{ m_RandomGenerator };
		uuids::uuid_name_generator m_UUIDNameGenerator{ m_UUIDGenerator() };

		static inline Context* s_Context = nullptr;
	};
}

#endif // NA_CONTEXT_HPP