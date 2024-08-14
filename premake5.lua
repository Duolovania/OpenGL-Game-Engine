workspace "OrbiterEngine"
    architecture "x64"

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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Glad"] = "Dependencies/Glad/include"
IncludeDir["OpenALSoft"] = "OrbiterCore/Vendor/openal-soft-master"

include "Dependencies/Glad"
include "OrbiterCore/Vendor/openal-soft-master"

project "OrbiterCore"
    location "OrbiterCore"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Targets all cpp and header files.
    files
    {
        "%{prj.name}/Scripts/**.h",
        "%{prj.name}/Scripts/**.cpp",

        "%{prj.name}/Vendor/stb_image/**cpp",
        "%{prj.name}/Vendor/stb_image/**h",

        "%{prj.name}/Vendor/glm/**cpp",
        "%{prj.name}/Vendor/glm/**h",

        "%{prj.name}/Vendor/imgui/**cpp",
        "%{prj.name}/Vendor/imgui/**h",
    }

    -- Includes dependencies and include paths.
    includedirs 
    {
        "%{prj.name}/Vendor/glfw-master-cherno/include/GLFW",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.OpenALSoft}",
        "%{IncludeDir.OpenALSoft}/include",
        "%{IncludeDir.OpenALSoft}/include/AL",
        "%{IncludeDir.OpenALSoft}/src",
        "%{IncludeDir.OpenALSoft}/src/alc",
        "%{IncludeDir.OpenALSoft}/src/al",
        "%{IncludeDir.OpenALSoft}/src/common",

        "%{prj.name}/Vendor",
        "%{prj.name}/Vendor/imgui",
        "%{prj.name}/Vendor/stb_image",
        "%{prj.name}/Vendor/glm",

        "%{prj.name}/Scripts/Headers",
        "%{prj.name}/Scripts/Tests",
        "%{prj.name}/Scripts"
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
        "OpenALSoft"
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
            "AL_LIBTYPE_STATIC"
        }

        -- -- Copies engine project DLL into editor project.
        -- postbuildcommands
        -- {
        --     ("{COPY} %{cfg.buildtarget.relpath} .. /bin/" .. outputdir .. "/OrbiterEditor")
        -- }

    filter "configurations:Debug"
        defines "OB_DEBUG"
        symbols "On"
        buildoptions "/MDd"

    filter "configurations:Release"
        defines "OB_RELEASE"
        optimize "On"
        buildoptions "/MD"

    filter "configurations:Dist"
        defines "OB_DIST"
        optimize "On"
        buildoptions "/MD"

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
        "%{prj.name}/Scripts/**.h",
        "%{prj.name}/Scripts/**.cpp"
    }

    -- Includes dependencies and include paths.
    includedirs
    {
        "%{IncludeDir.Glad}",
        "%{IncludeDir.OpenALSoft}",
        "%{IncludeDir.OpenALSoft}/include",

        "OrbiterCore/Vendor/glfw-master-cherno/include/GLFW",
        "OrbiterCore/Vendor/glm",
        "OrbiterCore/Vendor",
        "OrbiterCore/Scripts"
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
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "OB_DEBUG"
        symbols "On"
        buildoptions "/MDd"

    filter "configurations:Release"
        defines "OB_RELEASE"
        optimize "On"
        buildoptions "/MD"

    filter "configurations:Dist"
        defines "OB_DIST"
        optimize "On"
        buildoptions "/MD"