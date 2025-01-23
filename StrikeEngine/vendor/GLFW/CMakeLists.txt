cmake_minimum_required(VERSION 3.10)

project(GLFW)

set(CMAKE_CXX_STANDARD 17)
# Common source files
set(GLFW_SOURCES
    include/GLFW/glfw3.h
    include/GLFW/glfw3native.h
    src/context.c
    src/init.c
    src/input.c
    src/monitor.c

    src/null_init.c
    src/null_joystick.c
    src/null_monitor.c
    src/null_window.c

    src/platform.c
    src/vulkan.c
    src/window.c
)

# Create static library
add_library(GLFW STATIC ${GLFW_SOURCES})

# Include directories
target_include_directories(GLFW PUBLIC include)
target_include_directories(GLFW PRIVATE src)

# Disable warnings
target_compile_options(GLFW PRIVATE -w)

# Platform-specific configurations
if(UNIX AND NOT APPLE)
    # Linux
    target_sources(GLFW PRIVATE
        src/x11_init.c
        src/x11_monitor.c
        src/x11_window.c
        src/xkb_unicode.c
        src/posix_module.c
        src/posix_time.c
        src/posix_thread.c
        src/glx_context.c
        src/egl_context.c
        src/osmesa_context.c
        src/linux_joystick.c
    )
    target_compile_definitions(GLFW PRIVATE _GLFW_X11)
    set_target_properties(GLFW PROPERTIES POSITION_INDEPENDENT_CODE ON)
elseif(APPLE)
    # macOS
    target_sources(GLFW PRIVATE
        src/cocoa_init.m
        src/cocoa_monitor.m
        src/cocoa_window.m
        src/cocoa_joystick.m
        src/cocoa_time.c
        src/nsgl_context.m
        src/posix_thread.c
        src/posix_module.c
        src/osmesa_context.c
        src/egl_context.c
    )
    target_compile_definitions(GLFW PRIVATE _GLFW_COCOA)
    set_target_properties(GLFWPROPERTIES POSITION_INDEPENDENT_CODE ON)
elseif(WIN32)
    # Windows
    target_sources(GLFW PRIVATE
        src/win32_init.c
        src/win32_joystick.c
        src/win32_module.c
        src/win32_monitor.c
        src/win32_time.c
        src/win32_thread.c
        src/win32_window.c
        src/wgl_context.c
        src/egl_context.c
        src/osmesa_context.c
    )
    target_compile_definitions(GLFW PRIVATE 
        _GLFW_WIN32
        _CRT_SECURE_NO_WARNINGS
    )
endif()

set_target_properties(GLFW PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Configuration-specific settings
target_compile_options(GLFW PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Dist>:-O3>
)

# Set the runtime library for MSVC
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Special configuration for Debug-AS on Windows
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug-AS")
    target_compile_options(${PROJECT_NAME} PRIVATE /fsanitize=address)
    target_link_options(${PROJECT_NAME} PRIVATE /fsanitize=address)
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        LINK_FLAGS "/INCREMENTAL:NO"
    )
endif()

# Disable runtime checks for Debug-AS on Windows
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug-AS")
    string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif()