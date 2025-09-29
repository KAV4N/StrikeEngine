cmake_minimum_required(VERSION 3.16)
project(StrikeEngine)

set(CMAKE_CXX_STANDARD 17)

# Include vendor dependencies inside StrikeEngine
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# Assimp configuration
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

# Pugixml configuration
set(PUGIXML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PUGIXML_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# External dependencies
add_subdirectory(${INCLUDE_DIR}/GLFW)
add_subdirectory(${INCLUDE_DIR}/Glad)
add_subdirectory(${INCLUDE_DIR}/imgui)
add_subdirectory(${INCLUDE_DIR}/assimp)
add_subdirectory(${INCLUDE_DIR}/pugixml)

# Define the engine library
add_library(${PROJECT_NAME} STATIC)

# Precompiled headers
target_precompile_headers(${PROJECT_NAME} PRIVATE src/strikepch.h)

# Sources
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE VENDOR_SOURCES
    "${INCLUDE_DIR}/glm/glm/*.hpp"
    "${INCLUDE_DIR}/glm/glm/*.inl"
    "${INCLUDE_DIR}/stb_image/*.cpp"
    "${INCLUDE_DIR}/stb_image/*.h"
    "${INCLUDE_DIR}/pugixml/src/*.hpp"
    "${INCLUDE_DIR}/json/include/nlohmann/*.hpp"
    "${INCLUDE_DIR}/entt/single_include/entt/*.hpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES} ${VENDOR_SOURCES})

# Includes
target_include_directories(${PROJECT_NAME} PUBLIC
    src
    vendor/spdlog/include
    ${INCLUDE_DIR}/GLFW/include
    ${INCLUDE_DIR}/Glad/include
    ${INCLUDE_DIR}/ImGui
    ${INCLUDE_DIR}/glm
    ${INCLUDE_DIR}/stb_image
    ${INCLUDE_DIR}/json/include
    ${INCLUDE_DIR}/entt/single_include
    ${INCLUDE_DIR}/assimp/include
    ${INCLUDE_DIR}/pugixml/src
)

# Linking
target_link_libraries(${PROJECT_NAME} PUBLIC
    GLFW Glad ImGui assimp pugixml::pugixml opengl32
)

# Compile defs
target_compile_definitions(${PROJECT_NAME} PRIVATE
    CRT_SECURE_NO_WARNINGS
    PLATFORM_WINDOWS
    GLFW_INCLUDE_NONE
    $<$<CONFIG:Debug>:STRIKE_DEBUG>
    $<$<CONFIG:Release>:STRIKE_RELEASE>
    $<$<CONFIG:Dist>:STRIKE_DIST>
)

# Export engine resources path for dependent projects
set(STRIKE_ENGINE_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources CACHE PATH "StrikeEngine resources directory")

# Function to setup engine resources for a target
function(strike_engine_setup_resources TARGET_NAME)
    # Check if engine resources directory exists
    if(EXISTS ${STRIKE_ENGINE_RESOURCES_DIR})
        if(WIN32)
            # Debug: Create junction link for fast development
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/EngineResources
                COMMAND $<$<CONFIG:Debug>:cmd> /c $<$<CONFIG:Debug>:"mklink /J \"$<TARGET_FILE_DIR:${TARGET_NAME}>/EngineResources\" \"${STRIKE_ENGINE_RESOURCES_DIR}\"">
                COMMENT "Setting up StrikeEngine resources for Debug configuration"
            )
        else()
            # Debug: Create symlink for fast development
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove $<TARGET_FILE_DIR:${TARGET_NAME}>/EngineResources
                COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E create_symlink ${STRIKE_ENGINE_RESOURCES_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/EngineResources
                COMMENT "Setting up StrikeEngine resources for Debug configuration"
            )
        endif()

        # Release/Dist: Copy resources for distribution
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E remove_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/EngineResources
            COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E copy_directory ${STRIKE_ENGINE_RESOURCES_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/EngineResources
            COMMENT "Copying StrikeEngine resources for Release/Dist configuration"
        )
    else()
        message(WARNING "StrikeEngine resources directory not found at: ${STRIKE_ENGINE_RESOURCES_DIR}")
    endif()
endfunction()