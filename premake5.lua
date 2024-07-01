workspace "StrikeEngine"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDir = {}

includeDir["GLFW"] = "StrikeEngine/vendor/GLFW/include"
includeDir["Glad"] = "StrikeEngine/vendor/Glad/include"
includeDir["ImGui"] = "StrikeEngine/vendor/imgui"

include "StrikeEngine/vendor/GLFW"
include "StrikeEngine/vendor/Glad"
include "StrikeEngine/vendor/imgui"

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
        "%{includeDir.GLFW}",
        "%{includeDir.Glad}",
        "%{includeDir.ImGui}"
    }

    links {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }
   

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines {
            "PLATFORM_WINDOWS",
            "BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands {
            "copy /B /Y ..\\bin\\" .. outputDir .. "\\StrikeEngine\\StrikeEngine.dll ..\\bin\\" .. outputDir .. "\\Sandbox\\ > nul"
        }

    filter "configurations:Debug"
        defines "STRIKE_DEBUG"
        buildoptions "/MDd"
        staticruntime "off"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "STRIKE_RELEASE"
        buildoptions "/MD"
        staticruntime "off"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "STRIKE_DIST"
        buildoptions "/MD"
        staticruntime "off"
        runtime "Release"
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
        buildoptions "/MDd"
        symbols "On"

    filter "configurations:Release"
        defines "STRIKE_RELEASE"
        buildoptions "/MD"
        optimize "On"

    filter "configurations:Dist"
        defines "STRIKE_DIST"
        buildoptions "/MD"
        optimize "On"