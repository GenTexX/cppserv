project "spdlog"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
	staticruntime "on"

    targetdir("./bin/" .. outputDir .. "/%{prj.name}")
	objdir("./bin-int/" .. outputDir .. "/%{prj.name}")

    files { "src/**.cpp", "include/**.h" }

    includedirs { "include" }

    defines { "SPDLOG_COMPILED_LIB" }

	filter "system:windows"
		defines { "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING" }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:pre"
        defines { "NDEBUG" }
        optimize "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"