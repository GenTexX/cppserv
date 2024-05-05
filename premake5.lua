outputDir = "%{cfg.buildcfg}-%{cfg.architecture}"

workspace "cppserv"
    configurations { "debug", "release" }


group "dependencies"
include "lib/spdlog"
group ""

project "cppserv"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    architecture "x86_64"
    targetdir ("./bin/" .. outputDir .. "/%{prj.name}")
	objdir ("./bin-int/" .. outputDir .. "/%{prj.name}")
    files { "src/**.h", "src/**.hpp", "src/**.cpp" }
    system "linux"

    links {
    	"spdlog"
	}

    includedirs {
        "./lib/spdlog/include"
    }

    defines { "SPDLOG_COMPILED_LIB" }

    filter { "configurations:debug" }
        defines { "DEBUG" }
        symbols "On"

    filter { "configurations:release" }
        defines { "NDEBUG" }
        optimize "On"