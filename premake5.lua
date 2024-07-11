workspace "StrikeEngine"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDir = {}

includeDir["GLFW"] = "StrikeEngine/vendor/GLFW/include"
includeDir["Glad"] = "StrikeEngine/vendor/Glad/include"
includeDir["ImGui"] = "StrikeEngine/vendor/imgui"
includeDir["glm"] = "StrikeEngine/vendor/glm"

include "StrikeEngine/vendor/GLFW"
include "StrikeEngine/vendor/Glad"
include "StrikeEngine/vendor/imgui"

project "StrikeEngine"
    location "StrikeEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

    pchheader "strikepch.h"
    pchsource "StrikeEngine/src/strikepch.cpp"

    defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.glsl",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }
    
    includedirs {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{includeDir.GLFW}",
        "%{includeDir.Glad}",
        "%{includeDir.ImGui}",
        "%{includeDir.glm}"
    }

    links {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }
   

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

        defines {
            "PLATFORM_WINDOWS",
            "BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }
        --[[
        postbuildcommands {
            "copy /B /Y ..\\bin\\" .. outputDir .. "\\StrikeEngine\\StrikeEngine.dll ..\\bin\\" .. outputDir .. "\\Sandbox\\ > nul"
        }--]]

    filter "configurations:Debug"
        defines "STRIKE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "STRIKE_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "STRIKE_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

   

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "StrikeEngine/vendor/spdlog/include",
        "StrikeEngine/src",
        "%{includeDir.glm}",
        "%{includeDir.ImGui}"
    }

    links {
        "StrikeEngine"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "on"
        systemversion "latest"

        defines {
            "PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "STRIKE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "STRIKE_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "STRIKE_DIST"
        runtime "Release"
        optimize "on"