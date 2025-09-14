IncludeDirectories = {}
LibraryDirectories = {}
Libraries = {}

VulkanSdk = os.getenv("VULKAN_SDK")
IncludeDirectories["vk"] = "%{VulkanSdk}/Include/"
LibraryDirectories["vk"] = "%{VulkanSdk}/Lib/"

include "dependencies/fmt-Premake.lua"
include "dependencies/stb-Premake.lua"
include "dependencies/tinyobjloader-Premake.lua"
include "dependencies/GLFW-Premake.lua"
include "dependencies/imgui-Premake.lua"

IncludeDirectories["glm"] = "dependencies/glm/"
IncludeDirectories["nlohmann_json"] = "dependencies/nlohmann_json/include/"
IncludeDirectories["stduuid"] = "dependencies/stduuid/include/"

IncludeDirectories["openal"] = "dependencies/openal-soft/include/"
LibraryDirectories["openal"] = "dependencies/openal-soft/lib/"
Libraries["openal"] = "openal"

project "Natrium"
    location "./"
    targetname "%{prj.name}-bin"
    kind "StaticLib"
    staticruntime "Off"

    language "C++"
    cppdialect "C++20"
    systemversion "latest"

    pchheader "Pch.hpp"
    pchsource "src/Natrium/Pch.cpp"

    files {
        "include/Natrium/**.hpp",
        "src/Natrium/**.hpp",
        "src/Natrium/**.cpp"
    }

    includedirs {
        "%{IncludeDirectories.fmt}",
		"%{IncludeDirectories.stb}",
        "%{IncludeDirectories.glm}",
        "%{IncludeDirectories.nlohmann_json}",
		"%{IncludeDirectories.tiny_obj_loader}",
        "%{IncludeDirectories.glfw}",
        "%{IncludeDirectories.imgui}",
        "%{IncludeDirectories.stduuid}",
        "dependencies/",
        "include/",
        "src/Natrium/"
    }

    links {
        "%{Libraries.stb}",
        "%{Libraries.fmt}",
        "%{Libraries.tiny_obj_loader}",
        "%{Libraries.glfw}",
        "%{Libraries.imgui}",
    }

    filter "system:linux"
        links {
            "vulkan",
            "shaderc",
            "openal"
        }

        defines { "NA_PLATFORM_LINUX" }

    filter "system:windows"
        includedirs {
            "%{IncludeDirectories.vk}",
            "%{IncludeDirectories.openal}" 
        }

        libdirs {
            "%{LibraryDirectories.vk}",
            "%{LibraryDirectories.openal}" 
        }

        links {
            "vulkan-1",
            "winmm",
            "avrt",
            "user32",
            "ole32"
        }

        defines {
            "NA_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        }

        buildoptions { "/utf-8" }

    filter "toolset:clang"
      buildoptions { "-Wno-switch" }

    filter "configurations:dbg"
        symbols "On"
        runtime "Debug"
        defines { "NA_CONFIG_DEBUG" }

    filter "configurations:rel"
        optimize "speed"
        symbols "Off"
        defines { "NA_CONFIG_RELEASE" }

    filter "configurations:dist"
        optimize "speed"
        symbols "Off"
        defines { "NA_CONFIG_DIST" }

    filter { "system:windows", "configurations:dbg" }
        links {
            "openal-d",
            "shaderc_combinedd"
        }
    filter { "system:windows", "configurations:rel or dist" }
        links {
            "openal",
            "shaderc_combined"
        }
		
