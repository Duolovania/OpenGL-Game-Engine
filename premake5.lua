workspace "OpenGLEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "OpenGL"
    location "OpenGL"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Targets all cpp and header files.
    files
    {
        "%{prj.name}/Scripts/Headers/**.h",
        "%{prj.name}/Scripts/**.cpp"
    }

    -- Includes dependencies and include paths.
    includedirs 
    {
        "Dependencies/GLFW/include/GLFW",
        "Dependencies/GLEW/include/GL",
        "%{prj.name}/Scripts/Vendor",
        "%{prj.name}/Scripts/imgui",
        "%{prj.name}/Scripts",

        "%{prj.name}/Scripts/Vendor/glfw-master/include",
        "%{prj.name}/Scripts/Vendor/glm",
        "%{prj.name}/Scripts/Vendor/STB",
    }

    links
    {
        "Dependencies/GLFW/lib-vc2022/glfw3",
        "Dependencies/GLEW/lib/Release/Win32/glew32",
        "Dependencies/GLEW/lib/Release/Win32/glew32s",

        "Dependencies/GLEW/lib/Release/x64/glew32",
        "Dependencies/GLEW/lib/Release/x64/glew32s"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "WIN32"
        }

        ---- Copies engine project DLL into sandbox project.
        --postbuildcommands
        --{
            --("{COPY} %{cfg.buildtarget.relpath} .. /bin/" .. outputdir .. "/Sandbox")
        --}

    filter "configurations:Debug"
        defines "GL_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "GL_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "GL_DIST"
        optimize "On"

-- project "Sandbox"
--     location "Sandbox"
--     kind "ConsoleApp"
--     language "C++"

--     targetdir ("bin/" .. outputdir .. "/%{prj.name}")
--     objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

--     -- Targets all cpp and header files.
--     files
--     {
--         "%{prj.name}/Scripts/Headers/**.h",
--         "%{prj.name}/Scripts/**.cpp"
--     }

--     -- Includes dependencies and include paths.
--     include 
--     {
--         "Dependencies/GLFW/include/GLFW",
--         "Dependencies/GLEW/include/GL",
--         "Dependencies/GLFW/src",
--         "%{prj.name}/Scripts/Vendor"
--     }

--     links
--     {
--         "OpenGL"
--     }

--     filter "system:windows"
--         cppdialect "C++17"
--         staticruntime "On"
--         systemversion "latest"

--         defines
--         {
--             "WIN32"
--         }

--     filter "configurations:Debug"
--         defines "DEBUG"
--         symbols "On"

--     filter "configurations:Release"
--         defines "RELEASE"
--         optimize "On"

--     filter "configurations:Dist"
--         defines "DIST"
--         optimize "On"