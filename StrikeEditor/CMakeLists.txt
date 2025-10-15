cmake_minimum_required(VERSION 3.16)
project(StrikeEditor)

set(CMAKE_CXX_STANDARD 17)


add_executable(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_SOURCES 
    "src/*.cpp" 
    "src/*.h"
)

# Add sources to the executable
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/src
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/glm
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/imgui
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/entt/single_include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/Glad/include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/json/include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/assimp/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE StrikeEngine)

target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:STRIKE_DEBUG>
    $<$<CONFIG:Release>:STRIKE_RELEASE>
    $<$<CONFIG:Dist>:STRIKE_DIST>
)

# Output executables to Editor folder
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/StrikeEditor/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/StrikeEditor/Release"
    RUNTIME_OUTPUT_DIRECTORY_DIST "${CMAKE_BINARY_DIR}/StrikeEditor/Dist"
)
