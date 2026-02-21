cmake_minimum_required(VERSION 3.16)
project(Sandbox)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.h")
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE StrikeEngine)

strike_setup(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/Assets")