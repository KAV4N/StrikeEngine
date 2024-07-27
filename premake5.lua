workspace "StrikeEngine"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDir = {}

includeDir["GLFW"] = "StrikeEngine/vendor/GLFW/include"
includeDir["Glad"] = "StrikeEngine/vendor/Glad/include"
includeDir["ImGui"] = "StrikeEngine/vendor/imgui"
includeDir["json"] = "StrikeEngine/vendor/json/include"
includeDir["glm"] = "StrikeEngine/vendor/glm"
includeDir["stb"] = "StrikeEngine/vendor/stb"
includeDir["assimp"] = "StrikeEngine/vendor/assimp/include"

include "StrikeEngine/vendor/GLFW"
include "StrikeEngine/vendor/Glad"
include "StrikeEngine/vendor/imgui"
include "StrikeEngine/vendor/assimp"

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
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/json/include/nlohmann/**.hpp",
        "%{prj.name}/vendor/stb/**.h"
    }
    
    includedirs {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{includeDir.GLFW}",
        "%{includeDir.Glad}",
        "%{includeDir.ImGui}",
        "%{includeDir.glm}",
        "%{includeDir.stb}",
        "%{includeDir.assimp}",
        "%{includeDir.json}",
    }

    links {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
        "assimp"
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
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.glsl",
        "%{prj.name}/src/**.png"
    }

    includedirs {
        "StrikeEngine/vendor/spdlog/include",
        "StrikeEngine/src",
        "%{includeDir.glm}",
        "%{includeDir.ImGui}",
   
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