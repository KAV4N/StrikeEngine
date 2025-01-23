cmake_minimum_required(VERSION 3.10)

project(ImGui)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Create ImGui static library
add_library(ImGui STATIC
    imconfig.h
    imgui.h
    imgui.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    imgui_demo.cpp
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(ImGui PRIVATE WINDOWS)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(ImGui PRIVATE LINUX)
    set_target_properties(ImGui PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Configuration-specific settings
target_compile_options(ImGui PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Dist>:-O3>
)

# Disable generation of debug symbols for Dist configuration
if(CMAKE_BUILD_TYPE STREQUAL "Dist")
    set_target_properties(ImGui PROPERTIES COMPILE_FLAGS -g0)
endif()
