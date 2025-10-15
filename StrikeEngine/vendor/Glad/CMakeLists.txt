cmake_minimum_required(VERSION 3.16)
project(Glad)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(GLAD_SOURCES
    src/glad.c
    include/glad/glad.h
    include/KHR/khrplatform.h
)

# Create static library
add_library(Glad STATIC ${GLAD_SOURCES})

# Include directories
target_include_directories(Glad PUBLIC include)

# Set properties for different configurations
set_target_properties(Glad PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Set compile options for different configurations
target_compile_options(Glad PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
)

if(MSVC)
    target_compile_options(Glad PRIVATE /W4)
else()
    target_compile_options(Glad PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set the runtime library for MSVC
if(MSVC)
    set_property(TARGET Glad PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()