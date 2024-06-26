workspace "OpenGLEngine"
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
        -- "Dependencies/GLFW/Win32/include/GLFW",
        -- "Dependencies/GLFW/x64/include/GLFW",
        "%{prj.name}/Scripts/Vendor/glfw-master-cherno/include/GLFW",
        "Dependencies/GLEW/include/GL",

        "%{prj.name}/Scripts/Tests",

        "%{prj.name}/Scripts/Vendor",
        "%{prj.name}/Scripts/Vendor/imgui",
        "%{prj.name}/Scripts/Vendor/stb_image",
        "%{prj.name}/Scripts/Vendor/glm",
        "%{prj.name}/Scripts/Headers",
        "%{prj.name}/Scripts"
    }

    libdirs
    {
        "Dependencies/GLFW/Win32/lib-vc2022",
        "Dependencies/GLFW/x64/lib-vc2022",

        "Dependencies/GLEW/lib/Release/Win32",
        "Dependencies/GLEW/lib/Release/x64"
    }

    links
    {
        "glfw3",
        "opengl32",
        "Gdi32",
        "User32",
        "Shell32",
        "glew32s"
    }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "WIN32",
            "GLEW_STATIC"
        }

        ---- Copies engine project DLL into sandbox project.
        --postbuildcommands
        --{
            --("{COPY} %{cfg.buildtarget.relpath} .. /bin/" .. outputdir .. "/Sandbox")
        --}

    filter "configurations:Debug"
        defines "GL_DEBUG"
        symbols "On"
        buildoptions "/MDd"

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