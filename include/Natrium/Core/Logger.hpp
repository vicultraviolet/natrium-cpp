#if !defined(NA_LOGGER_HPP)
#define NA_LOGGER_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Core/ANSI_EscapeCodes.hpp"

namespace Na {
	enum LogLevel : u8 {
		None = 0,
		Trace, Debug, Info, Notice, Warn, Error, Fatal,
		Last = Fatal
	};
	constexpr std::array<std::string_view, LogLevel::Last + 1> k_LogLevelNames = {
		"[None]", "[Trace]", "[Debug]", "[Info]", "[Notice]", "[Warn]", "[Error]", "[Fatal]"
	};

	inline std::string_view LogLevelEscapeCodes(LogLevel level)
	{
		switch (level)
		{
		case Trace:
			return Na::ANSI_EscapeCodes::k_Bold;
		case Debug:
			return NA_CONCAT_STR_VIEW(
				Na::ANSI_EscapeCodes::k_Bold,
				Na::ANSI_EscapeCodes::Foreground::k_Green
			);
		case Info:
			return NA_CONCAT_STR_VIEW(
				Na::ANSI_EscapeCodes::k_Bold,
				Na::ANSI_EscapeCodes::Foreground::k_Blue
			);
		case Notice:
			return NA_CONCAT_STR_VIEW(
				Na::ANSI_EscapeCodes::k_Bold,
				Na::ANSI_EscapeCodes::Foreground::k_Magenta
			);
		case Warn: 
			return NA_CONCAT_STR_VIEW(
				Na::ANSI_EscapeCodes::k_Bold,
				Na::ANSI_EscapeCodes::Foreground::k_Yellow
			);
		case Error:
			return NA_CONCAT_STR_VIEW(
				Na::ANSI_EscapeCodes::k_Bold,
				Na::ANSI_EscapeCodes::Foreground::k_Red
			);
		case Fatal:
			return NA_CONCAT_STR_VIEW(
				Na::ANSI_EscapeCodes::k_Bold,
				Na::ANSI_EscapeCodes::Background::k_Red
			);
		}
		return "";
	}

#if !defined(NA_CONFIG_DIST)
	template<typename t_Stream = std::ostream, bool t_Enabled = true>
#else
	template<typename t_Stream = std::ostream, bool t_Enabled = false>
#endif 
	class Logger {
	public:
		std::string_view name;
		t_Stream* stream;
	public:
		inline void log(LogLevel level, const std::string_view& msg)
		{
			if (!t_Enabled)
				return;

			*this->stream << NA_FORMAT(
				"{}[{:%H:%M:%S}]{}[{}]:{} {}\n",
				LogLevelEscapeCodes(level),
				std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now()),
				k_LogLevelNames[level],
				this->name,
				Na::ANSI_EscapeCodes::k_Reset,
				msg
			);
		}

		template<typename... t_Args>
		inline void fmt(LogLevel level, fmt::format_string<t_Args...> str, t_Args&&... __args)
		{
			if (!t_Enabled)
				return;

			*this->stream << NA_FORMAT(
				"{}[{:%H:%M:%S}]{}[{}]:{} {}\n",
				LogLevelEscapeCodes(level),
				std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now()),
				k_LogLevelNames[level],
				this->name,
				Na::ANSI_EscapeCodes::k_Reset,
				NA_FORMAT(str, std::forward<t_Args>(__args)...)
			);
		}

		inline void header(LogLevel level = LogLevel::Info)
		{
			if (!t_Enabled)
				return;

			*this->stream << NA_FORMAT(
				"{}[{:%Y-%m-%d %H:%M:%S}][{}]{}\n",
				LogLevelEscapeCodes(level),
				std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now()),
			#if __cpp_lib_chrono >= 201907L
				std::chrono::current_zone()->name(),
			#else
				"",
			#endif
				Na::ANSI_EscapeCodes::k_Reset
			);
		}

		inline void new_line(void)
		{
			if (!t_Enabled)
				return;

			*stream << '\n';
		}

		inline void operator()(LogLevel level, const std::string_view& msg)
		{
			if (!t_Enabled)
				return;

			this->log(level, msg);
		}

		[[nodiscard]] inline constexpr bool enabled(void) { return t_Enabled; }
	};
	inline Logger<> g_Logger{"Natrium", &std::clog};
} // namespace Na

#endif // NA_LOGGER_HPP