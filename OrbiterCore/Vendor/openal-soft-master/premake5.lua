project "OpenALSoft"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	defines
	{
		"AL_ALEXT_PROTOTYPES",
		"AL_LIBTYPE_STATIC"
	}

	includedirs
	{
		"src",
		"src/alc",
		"src/al",
		"src/common",
		"include",
		"include/AL"
	}

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	excludes
	{
		"src/alc/mixer/mixer_neon.cpp"
	}
	
	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

		defines
		{
			"WIN32",
			"_WINDOWS",
			"AL_BUILD_LIBRARY",
			"AL_ALEXT_PROTOTYPES",
			"_WIN32",
			"_WIN32_WINNT=0x0502",
			"_CRT_SECURE_NO_WARNINGS",
			"NOMINMAX",
			"CMAKE_INTDIR=\"Debug\"",
			"OpenAL_EXPORTS"
		}

		links
		{
			"winmm"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		buildoptions "/MDd"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		buildoptions "/MD"
