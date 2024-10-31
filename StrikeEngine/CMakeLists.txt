cmake_minimum_required(VERSION 3.16)

project(StrikeEngine)
set(CMAKE_CXX_STANDARD 17)

# Precompiled headers
add_library(${PROJECT_NAME} STATIC)  # Define the library first
target_precompile_headers(${PROJECT_NAME} PRIVATE src/strikepch.h)



file(GLOB_RECURSE PROJECT_SOURCES 
    "src/*.cpp" 
    "src/*.h" 
)


file(GLOB_RECURSE VENDOR_SOURCES 
    "${INCLUDE_DIR}/glm/glm/*.hpp"
    "${INCLUDE_DIR}/glm/glm/*.inl"
	
	"${INCLUDE_DIR}/stb_image/*.cpp"
    "${INCLUDE_DIR}/stb_image/*.h"
	
    "${INCLUDE_DIR}/json/include/nlohmann/*.hpp"
    "${INCLUDE_DIR}/entt/single_include/entt/*.hpp"
)

# Add sources to the library
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES} ${VENDOR_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC 
    src
    vendor/spdlog/include
    ${INCLUDE_DIR}/GLFW/include
    ${INCLUDE_DIR}/Glad/include
    ${INCLUDE_DIR}/ImGui
    ${INCLUDE_DIR}/glm
    ${INCLUDE_DIR}/stb_image
    ${INCLUDE_DIR}/assimp/include
    ${INCLUDE_DIR}/json/include
    ${INCLUDE_DIR}/entt/single_include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    GLFW
    Glad
    ImGui
    assimp
    opengl32
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    PLATFORM_WINDOWS
    BUILD_DLL  # TODO remove this not used
	GLFW_INCLUDE_NONE
)

# Set configuration-specific compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:STRIKE_DEBUG>
    $<$<CONFIG:Release>:STRIKE_RELEASE>
    $<$<CONFIG:Dist>:STRIKE_DIST>
)
