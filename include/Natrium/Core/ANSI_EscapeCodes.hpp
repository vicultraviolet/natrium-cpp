#if !defined(ANSI_ESCAPE_CODES_HPP)
#define ANSI_ESCAPE_CODES_HPP

#include "Natrium/Core.hpp"

namespace Na {
	namespace ANSI_EscapeCodes {
		inline constexpr std::string_view k_Reset = "\x1B[0m";
		inline constexpr std::string_view k_Bold = "\x1B[1m";
		inline constexpr std::string_view k_Dim = "\x1B[2m";
		inline constexpr std::string_view k_Italic = "\x1B[3m";
		inline constexpr std::string_view k_Underline = "\x1B[4m";
		inline constexpr std::string_view k_Blink = "\x1B[5m";
		inline constexpr std::string_view k_Invert = "\x1B[7m";
		inline constexpr std::string_view k_Hidden = "\x1B[8m";
		inline constexpr std::string_view k_Strikethrough = "\x1B[9m";

		namespace Foreground {
			inline constexpr std::string_view k_Black = "\x1B[30m";
			inline constexpr std::string_view k_Red = "\x1B[31m";
			inline constexpr std::string_view k_Green = "\x1B[32m";
			inline constexpr std::string_view k_Yellow = "\x1B[33m";
			inline constexpr std::string_view k_Blue = "\x1B[34m";
			inline constexpr std::string_view k_Magenta = "\x1B[35m";
			inline constexpr std::string_view k_Cyan = "\x1B[36m";
			inline constexpr std::string_view k_White = "\x1B[37m";
			inline constexpr std::string_view k_Default = "\x1B[39m";

			inline constexpr std::string_view k_BrightBlack = "\x1B[90m";
			inline constexpr std::string_view k_BrightRed = "\x1B[91m";
			inline constexpr std::string_view k_BrightGreen = "\x1B[92m";
			inline constexpr std::string_view k_BrightYellow = "\x1B[93m";
			inline constexpr std::string_view k_BrightBlue = "\x1B[94m";
			inline constexpr std::string_view k_BrightMagenta = "\x1B[95m";
			inline constexpr std::string_view k_BrightCyan = "\x1B[96m";
			inline constexpr std::string_view k_BrightWhite = "\x1B[97m";
		} // namespace Foreground

		namespace Background {
			inline constexpr std::string_view k_Black = "\x1B[40m";
			inline constexpr std::string_view k_Red = "\x1B[41m";
			inline constexpr std::string_view k_Green = "\x1B[42m";
			inline constexpr std::string_view k_Yellow = "\x1B[43m";
			inline constexpr std::string_view k_Blue = "\x1B[44m";
			inline constexpr std::string_view k_Magenta = "\x1B[45m";
			inline constexpr std::string_view k_Cyan = "\x1B[46m";
			inline constexpr std::string_view k_White = "\x1B[47m";
			inline constexpr std::string_view k_Default = "\x1B[49m";

			inline constexpr std::string_view k_BrightBlack = "\x1B[100m";
			inline constexpr std::string_view k_BrightRed = "\x1B[101m";
			inline constexpr std::string_view k_BrightGreen = "\x1B[102m";
			inline constexpr std::string_view k_BrightYellow = "\x1B[103m";
			inline constexpr std::string_view k_BrightBlue = "\x1B[104m";
			inline constexpr std::string_view k_BrightMagenta = "\x1B[105m";
			inline constexpr std::string_view k_BrightCyan = "\x1B[106m";
			inline constexpr std::string_view k_BrightWhite = "\x1B[107m";
		} // namespace Background
	} // namespace ANSI_EscapeCodes
} // namespace Na

#endif // ANSI_ESCAPE_CODES_HPP