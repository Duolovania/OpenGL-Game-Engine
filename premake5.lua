workspace "OrbiterEngine"
    architecture "x64"
    startproject "OrbiterEditor" -- Sets the startup project to the editor.

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    platforms
    {
        "Win32",
        "x64"
    }

    filter { "platforms:Win32" }
        system "Windows"
        architecture "x86"

    filter { "platforms:x64" }
        system "Windows"
        architecture "x64"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" -- Sets the output directory naming structure.

IncludeDir = {}
IncludeDir["Glad"] = "Dependencies/Glad/include"
IncludeDir["yaml_cpp"] = "OrbiterCore/Vendor/yaml-cpp-master/include"
IncludeDir["OpenALSoft"] = "OrbiterCore/Vendor/openal-soft-master/include"

-- Includes folders in the project.
include "Dependencies/Glad"
include "OrbiterCore/Vendor/openal-soft-master"
include "OrbiterCore/Vendor/yaml-cpp-master"

-- Engine core configuration.
project "OrbiterCore"
    location "OrbiterCore"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Files to be compiled.
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",

        "%{prj.name}/Vendor/stb_image/**cpp",
        "%{prj.name}/Vendor/stb_image/**h",

        "%{prj.name}/Vendor/glm/**cpp",
        "%{prj.name}/Vendor/glm/**h",

        "%{prj.name}/Vendor/imgui/**cpp",
        "%{prj.name}/Vendor/imgui/**h",

        "%{prj.name}/Vendor/tinyfiledialogs/**c",
        "%{prj.name}/Vendor/tinyfiledialogs/**h"
    }

    -- Include directories/paths.
    includedirs 
    {
        "%{IncludeDir.Glad}",
        "%{IncludeDir.yaml_cpp}",

        "%{IncludeDir.OpenALSoft}",
        "%{IncludeDir.OpenALSoft}/AL",
        
        "%{prj.name}/Vendor",
        "%{prj.name}/Vendor/glfw-master-cherno/include/GLFW",
        "%{prj.name}/Vendor/imgui",
        "%{prj.name}/Vendor/stb_image",
        "%{prj.name}/Vendor/glm",

        "%{prj.name}/Vendor/tinyfiledialogs",
        "%{prj.name}/Vendor/tinyfiledialogs/more_dialogs",

        "%{prj.name}/src/include",
        "%{prj.name}/src/Tests",
        "%{prj.name}/src"
    }

    excludes
	{
		"%{IncludeDir.OpenALSoft}/src/alc/mixer/mixer_neon.cpp"
	}

    links
    {
        "glfw3",
        "opengl32",
        "Gdi32",
        "User32",
        "Shell32",
        "Glad",
        "OpenALSoft",
        "yaml-cpp"
    }

    filter { "platforms:Win32" }
        libdirs
        {
            "Dependencies/GLFW/Win32/lib-vc2022",
        }

    filter { "platforms:x64" }
        libdirs
        {
            "Dependencies/GLFW/x64/lib-vc2022",
        }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "WIN32",
            "GLFW_INCLUDE_NONE",
            "_CRT_SECURE_NO_WARNINGS",
            "NOMINMAX",
            "AL_LIBTYPE_STATIC",
            "YAML_CPP_STATIC_DEFINE"
        }

        -- -- Copies engine project DLL into editor project.
        -- postbuildcommands
        -- {
        --     "{COPY} %{cfg.buildtarget.relpath} .. /bin/" .. outputdir .. "/OrbiterEditor"
        -- }

    filter "configurations:Debug"
        defines "OB_DEBUG"
        symbols "On"
        buildoptions "/MDd" -- Multi-threaded debug DLL.

    filter "configurations:Release"
        defines "OB_RELEASE"
        optimize "On"
        buildoptions "/MD" -- Multi-threaded DLL.

    filter "configurations:Dist"
        defines "OB_DIST"
        optimize "On"
        buildoptions "/MD" -- Multi-threaded DLL.

-- Engine editor configuration.
project "OrbiterEditor"
    location "OrbiterEditor"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    dependson { "OrbiterCore" }

    -- Targets all cpp and header files.
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    -- Includes dependencies and include paths.
    includedirs
    {
        "%{IncludeDir.Glad}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.OpenALSoft}",
        "%{IncludeDir.OpenALSoft}/include",

        "OrbiterCore/Vendor/glfw-master-cherno/include/GLFW",
        "OrbiterCore/Vendor/glm",
        "OrbiterCore/Vendor",

        "OrbiterCore/src",
        "OrbiterCore/src/include",
        "%{prj.name}/src/include"
    }

    links
    {
        "OrbiterCore"
    }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "WIN32",
            "_CRT_SECURE_NO_WARNINGS",
            "NOMINMAX",
            "AL_LIBTYPE_STATIC",
            "GLFW_INCLUDE_NONE",
            "YAML_CPP_STATIC_DEFINE"
        }

    filter "configurations:Debug"
        defines "OB_DEBUG"
        symbols "On"
        buildoptions "/MDd" -- Multi-threaded debug DLL.

    filter "configurations:Release"
        defines "OB_RELEASE"
        optimize "On"
        buildoptions "/MD" -- Multi-threaded DLL.

    filter "configurations:Dist"
        defines "OB_DIST"
        optimize "On"
        buildoptions "/MD" -- Multi-threaded DLL.