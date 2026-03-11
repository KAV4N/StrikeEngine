cmake_minimum_required(VERSION 3.16)
project(StrikeProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wno-unknown-pragmas
        -Wno-deprecated-copy
        -Wno-unused-parameter
    )
endif()

add_subdirectory(StrikeEngine)
add_subdirectory(Sandbox)

option(STRIKE_BUILD_TESTS "Build StrikeEngine tests" ON)
if(STRIKE_BUILD_TESTS)
    target_compile_definitions(StrikeEngine PUBLIC STRIKE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()