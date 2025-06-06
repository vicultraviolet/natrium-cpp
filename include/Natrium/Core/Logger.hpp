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
	template<bool t_Enabled = true>
#else
	template<bool t_Enabled = false>
#endif 
	class Logger {
	public:
		std::string_view name;

		template<typename t_Stream>
		void print_raw_to(t_Stream& stream, const std::string_view& msg) const
		{
			if (!t_Enabled)
				return;
			stream << msg;
		}

		void print_raw(const std::string_view& msg) const
		{
			if (!t_Enabled)
				return;
			this->print_raw_to(std::cout, msg);
		}

		template<typename t_Stream, typename... t_Args>
		void printf_raw_to(
			t_Stream& stream,
			fmt::format_string<t_Args...> str,
			t_Args&&... __args
		) const
		{
			if (!t_Enabled)
				return;
			stream << NA_FORMAT(str, std::forward<t_Args>(__args)...);
		}

		template<typename... t_Args>
		void printf_raw(fmt::format_string<t_Args...> str, t_Args&&... __args) const
		{
			if (!t_Enabled)
				return;
			this->printf_raw_to(std::cout, str, std::forward<t_Args>(__args)...);
		}

		template<typename t_Stream>
		void print_to(t_Stream& stream, LogLevel level, const std::string_view& msg) const
		{
			if (!t_Enabled)
				return;
			this->printf_raw_to(stream, "{}[{:%H:%M:%S}]{}[{}]:{} {}\n",
				LogLevelEscapeCodes(level),
				std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now()),
				k_LogLevelNames[level],
				this->name,
				Na::ANSI_EscapeCodes::k_Reset,
				msg
			);
		}

		void print(LogLevel level, const std::string_view& msg) const
		{
			if (!t_Enabled)
				return;
			this->print_to(std::cout, level, msg);
		}

		template<typename t_Stream, typename... t_Args>
		void printf_to(
			t_Stream& stream,
			LogLevel level,
			fmt::format_string<t_Args...> str,
			t_Args&&... __args
		) const
		{
			if (!t_Enabled)
				return;
			this->print_to(stream, level, NA_FORMAT(str, std::forward<t_Args>(__args)...));
		}

		template<typename... t_Args>
		void printf(LogLevel level, fmt::format_string<t_Args...> str, t_Args&&... __args) const
		{
			if (!t_Enabled)
				return;
			this->printf_to(std::cout, level, str, std::forward<t_Args>(__args)...);
		}

		template<typename t_Stream>
		void print_header_to(t_Stream& stream, LogLevel level = Info) const
		{
			if (!t_Enabled)
				return;
			this->printf_raw_to(stream, "{}[{:%Y-%m-%d %H:%M:%S}][{}]{}\n",
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

		void print_header(LogLevel level = Info) const
		{
			if (!t_Enabled)
				return;
			this->print_header_to(std::cout, level);
		}

		[[nodiscard]] inline constexpr bool enabled(void) const { return t_Enabled; }
	};
	inline Logger<> g_Logger{"Natrium"};
} // namespace Na

#endif // NA_LOGGER_HPP