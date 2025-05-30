#if !defined(NA_CORE_HPP)
#define NA_CORE_HPP

#include <stdint.h>

#define NA_BIT(x) (1u << x)
#define NA_GET_FROM_OFFSET(t, p) (t*)((Na::Byte*)p - offsetof(t, p))

#define NA_FORMAT fmt::format

#if !defined(NA_CONFIG_DEBUG) && !defined(NA_CONFIG_RELEASE) && !defined(NA_CONFIG_DIST)
    #error "Has not defined a build configuration macro!"
    #error "Define NA_CONFIG_DEBUG, NA_CONFIG_RELEASE or NA_CONFIG_DIST!"
#endif // NA_CONFIG

#if !defined(NA_PLATFORM_WINDOWS) && !defined(NA_PLATFORM_LINUX)
    #error "Has not defined a platform macro!"
    #error "Define NA_PLATFORM_WINDOWS or NA_PLATFORM_LINUX!"
#endif // NA_PLATFORM

#define NA_VERIFY(x, ...) \
    if(!(x)) {\
        throw std::runtime_error(NA_FORMAT(__VA_ARGS__));\
    }

#if defined(NA_CONFIG_DIST)
    #define NA_ASSERT(x, ...)
#else
    #define NA_ASSERT NA_VERIFY
#endif // NA_CONFIG

#if defined(NA_PLATFORM_WINDOWS) && defined(NA_CONFIG_DIST)
    #define NA_WINDOWED_APP
#endif

// signed integers
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

constexpr i8  k_I8Max  = INT8_MAX;
constexpr i16 k_I16Max = INT16_MAX;
constexpr i32 k_I32Max = INT32_MAX;
constexpr i64 k_I64Max = INT64_MAX;

// unsigned integers
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

constexpr u8  k_U8Max  = UINT8_MAX;
constexpr u16 k_U16Max = UINT16_MAX;
constexpr u32 k_U32Max = UINT32_MAX;
constexpr u64 k_U64Max = UINT64_MAX;

// booleans
using b8  = i8;
using b32 = i32;

constexpr b8 k_False = 0;
constexpr b8 k_True = 1;

using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

#define NA_CONCAT_STR_VIEW(sv1, sv2) StringViewCat<sv1, sv2>::data

namespace Na {
    using Byte = unsigned char;

    constexpr u64 k_InvalidHandle = k_U64Max;

    enum class BuildConfig : u8 {
        None = 0, Debug, Release, Distribution
    };

#if defined(NA_CONFIG_DEBUG)
    constexpr BuildConfig k_BuildConfig = BuildConfig::Debug;
#elif defined(NA_CONFIG_RELEASE)
    constexpr BuildConfig k_BuildConfig = BuildConfig::Release;
#elif defined(NA_CONFIG_DIST)
    constexpr BuildConfig k_BuildConfig = BuildConfig::Distribution;
#else
    constexpr BuildConfig k_BuildConfig = BuildConfig::None;
#endif // NA_CONFIG

    enum class Platform : u8 {
        None = 0, Windows, Linux
    };

#if defined(NA_PLATFORM_WINDOWS)
    constexpr Platform k_Platform = Platform::Windows;
#elif defined(NA_PLATFORM_LINUX)
    constexpr Platform k_Platform = Platform::Linux;
#else
    constexpr Platform k_Platform = Platform::None;
#endif // NA_PLATFORM

#if defined(NA_PLATFORM_WINDOWS) && defined(NA_CONFIG_DIST)
    constexpr bool k_WindowedApp = true;
#else
    constexpr bool k_WindowedApp = false;
#endif

    template<const std::string_view& t_Str1 = "", const std::string_view& t_Str2 = "">
    class StringViewCat {
    private:
        static constexpr auto _GetArray(void)
        {
            std::array<char, t_Str1.size() + t_Str2.size()> array;
            for (u64 i = 0; i < t_Str1.size(); i++)
                array[i] = t_Str1[i];
            for (u64 i = 0; i < t_Str2.size(); i++)
                array[i + t_Str1.size()] = t_Str2[i];

            return array;
        }
        constexpr static std::array<char, t_Str1.size() + t_Str2.size()> s_Array = _GetArray();
    public:
        constexpr static std::string_view data{ s_Array.data(), s_Array.size() };
    };

    [[nodiscard]] inline i32 Round32(float num) { return (i32)floor(num + 0.5f); }
    [[nodiscard]] inline i64 Round64(double num) { return (i64)floor(num + 0.5); }

    template<typename T>
    [[nodiscard]] inline T* tmalloc(u64 count = 1)
    {
        return (T*)malloc(count * sizeof(T));
    }
    
    template<typename T>
    [[nodiscard]] inline T* tcalloc(u64 count = 1)
    {
        return (T*)calloc(count, sizeof(T));
    }
    
    template<typename T>
    [[nodiscard]] inline T* trealloc(T* buffer, u64 count)
    {
        return (T*)realloc(buffer, count * sizeof(T));
    }

} // namespace Na

#endif // NA_CORE_HPP