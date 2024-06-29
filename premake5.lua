workspace "StrikeEngine"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDir = {}

includeDir["GLFW"] = "StrikeEngine/vendor/GLFW/include"
include "StrikeEngine/vendor/GLFW"

project "StrikeEngine"
    location "StrikeEngine"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

    pchheader "strikepch.h"
    pchsource "StrikeEngine/src/strikepch.cpp"
        
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{includeDir.GLFW}"
    }

    links {
        "GLFW",
        "opengl32.lib"
    }
   

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines {
            "PLATFORM_WINDOWS",
            "BUILD_DLL"
        }

        postbuildcommands {
            "copy /B /Y ..\\bin\\" .. outputDir .. "\\StrikeEngine\\StrikeEngine.dll ..\\bin\\" .. outputDir .. "\\Sandbox\\ > nul"
        }

    filter "configurations:Debug"
        defines "STRIKE_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "STRIKE_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "STRIKE_DIST"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

   

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "StrikeEngine/vendor/spdlog/include",
        "StrikeEngine/src"
    }

    links {
        "StrikeEngine"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines {
            "PLATFORM_WINDOWS"
        }

        postbuildcommands {
            "copy /B /Y ..\\bin\\" .. outputDir .. "\\StrikeEngine\\StrikeEngine.dll ..\\bin\\" .. outputDir .. "\\Sandbox\\ > nul"
        }

    filter "configurations:Debug"
        defines "STRIKE_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "STRIKE_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "STRIKE_DIST"
        optimize "On"