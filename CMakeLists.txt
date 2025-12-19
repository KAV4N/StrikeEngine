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




