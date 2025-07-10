#if !defined(NA_CORE_HPP)
#define NA_CORE_HPP

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

using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

#define NA_CONCAT_STR_VIEW(sv1, sv2) StringViewCat<sv1, sv2>::data

namespace Na {
    namespace Primitives {
        using i8  = char;
        using i16 = short;
        using i32 = int;
        using i64 = long long;

        constexpr i8  i8max  = INT8_MAX;
        constexpr i16 i16max = INT16_MAX;
        constexpr i32 i32max = INT32_MAX;
        constexpr i64 i64max = INT64_MAX;

        using s8  = signed char;
        using s16 = signed short;
        using s32 = signed int;
        using s64 = signed long long;

        constexpr s8  s8max  = INT8_MAX;
        constexpr s16 s16max = INT16_MAX;
        constexpr s32 s32max = INT32_MAX;
        constexpr s64 s64max = INT64_MAX;

        using u8  = unsigned char;
        using u16 = unsigned short;
        using u32 = unsigned int;
        using u64 = unsigned long long;

        constexpr u8  u8max  = UINT8_MAX;
        constexpr u16 u16max = UINT16_MAX;
        constexpr u32 u32max = UINT32_MAX;
        constexpr u64 u64max = UINT64_MAX;

        using f32 = float;
        using f64 = double;

        using b8 = i8;
        using b32 = i32;

        constexpr b8 k_False = 0;
        constexpr b8 k_True  = 1;

        using Byte = unsigned char;

        using handle_t = u64;
        constexpr u64 k_InvalidHandle = u64max;
    } // namespace Primitives
    using namespace Primitives;

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

    enum class init { init };
} // namespace Na

#endif // NA_CORE_HPP