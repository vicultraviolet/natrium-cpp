IncludeDirectories["imgui"] = "dependencies/imgui/"
Libraries["imgui"] = "imgui"

project "imgui"
    location "./"
    targetname "%{prj.name}-bin"
    kind "StaticLib"
    staticruntime "Off"

    language "C++"
    cppdialect "C++20"
    systemversion "latest"

    files {
        "imgui/imconfig.h",

        "imgui/imgui.h",
        "imgui/imgui.cpp",

        "imgui/imgui_internal.h",
        "imgui/imstb_rectpack.h",
        "imgui/imstb_textedit.h",
        "imgui/imstb_truetype.h",

        "imgui/imgui_demo.cpp",
        "imgui/imgui_draw.cpp",
        "imgui/imgui_tables.cpp",
        "imgui/imgui_widgets.cpp",

        "imgui/backends/imgui_impl_glfw.h",
        "imgui/backends/imgui_impl_glfw.cpp",

        "imgui/backends/imgui_impl_vulkan.h",
        "imgui/backends/imgui_impl_vulkan.cpp"
    }

    includedirs {
        "./",
        "imgui/",
        "imgui/backends",
        "../%{IncludeDirectories.glfw}"
    }

	filter "system:linux"
        pic "On"

    filter "system:windows"
        includedirs "%{IncludeDirectories.vk}" 

        buildoptions {
            "/utf-8"
        }

    filter "configurations:dbg"
        symbols "On"
        runtime "Debug"

    filter "configurations:rel"
        optimize "speed"
        symbols "Off"

    filter "configurations:dist"
        optimize "speed"
        symbols "Off"
