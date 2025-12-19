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

# Bullet Physics configuration
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(USE_GLUT OFF CACHE BOOL "" FORCE)
set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "" FORCE)
set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)



# External dependencies
add_subdirectory(${INCLUDE_DIR}/GLFW)
add_subdirectory(${INCLUDE_DIR}/Glad)
add_subdirectory(${INCLUDE_DIR}/assimp)
add_subdirectory(${INCLUDE_DIR}/pugixml)
add_subdirectory(${INCLUDE_DIR}/bullet3)
add_subdirectory(${INCLUDE_DIR}/freetype)

# ============================================================================
# SHADER STRINGIFICATION
# ============================================================================
set(SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders)
set(GENERATED_SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(GENERATED_SHADERS_HEADER ${GENERATED_SHADERS_DIR}/GenShaders.h)

# Collect all shader files
file(GLOB_RECURSE SHADER_FILES 
    "${SHADERS_DIR}/*.glsl"
    "${SHADERS_DIR}/*.shader"
)

# Function to convert shader filename to valid C++ identifier
function(shader_to_identifier SHADER_PATH OUTPUT_VAR)
    get_filename_component(SHADER_NAME ${SHADER_PATH} NAME)
    string(REPLACE "." "_" IDENTIFIER ${SHADER_NAME})
    string(REPLACE "-" "_" IDENTIFIER ${IDENTIFIER})
    set(${OUTPUT_VAR} ${IDENTIFIER} PARENT_SCOPE)
endfunction()

# --------------------------------------------------------------------
# Generate header shaders
# --------------------------------------------------------------------
file(WRITE ${GENERATED_SHADERS_HEADER}
"// Auto-generated file - DO NOT EDIT\n"
"// Generated from shaders in ${SHADERS_DIR}\n\n"
"#pragma once\n\n"
"#include <string>\n"
"#include <unordered_map>\n\n"
"namespace StrikeEngine {\n\n"   
)

# Add each shader as a string literal
foreach(SHADER_FILE ${SHADER_FILES})
    shader_to_identifier(${SHADER_FILE} SHADER_IDENTIFIER)

    # Read shader content
    file(READ ${SHADER_FILE} SHADER_CONTENT)

    # Escape special characters
    string(REPLACE "\\" "\\\\" SHADER_CONTENT "${SHADER_CONTENT}")
    string(REPLACE "\"" "\\\"" SHADER_CONTENT "${SHADER_CONTENT}")
    string(REPLACE "\n" "\\n\"\n\"" SHADER_CONTENT "${SHADER_CONTENT}")

    # Write to header
    file(APPEND ${GENERATED_SHADERS_HEADER}
"inline const char* ${SHADER_IDENTIFIER} = \n\"${SHADER_CONTENT}\";\n\n"
    )
endforeach()

# Create shader map for easy access by filename
file(APPEND ${GENERATED_SHADERS_HEADER}
"// Shader lookup map\n"
"inline const std::unordered_map<std::string, const char*> shaderMap = {\n"
)

foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    shader_to_identifier(${SHADER_FILE} SHADER_IDENTIFIER)
    file(APPEND ${GENERATED_SHADERS_HEADER}
"    {\"${SHADER_NAME}\", ${SHADER_IDENTIFIER}},\n"
    )
endforeach()

file(APPEND ${GENERATED_SHADERS_HEADER}
"};\n\n"
"// Helper function to get shader source by filename\n"
"inline const char* getShader(const std::string& filename) {\n"
"    auto it = shaderMap.find(filename);\n"
"    return (it != shaderMap.end()) ? it->second : nullptr;\n"
"}\n\n"
"} // namespace StrikeEngine\n"   
)

message(STATUS "Generated shader header: ${GENERATED_SHADERS_HEADER}")

# ============================================================================

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
    ${INCLUDE_DIR}/glm
    ${INCLUDE_DIR}/stb_image
    ${INCLUDE_DIR}/json/include
    ${INCLUDE_DIR}/entt/single_include
    ${INCLUDE_DIR}/assimp/include
    ${INCLUDE_DIR}/pugixml/src
    ${INCLUDE_DIR}/bullet3/src
    ${INCLUDE_DIR}/miniaudio
    ${GENERATED_SHADERS_DIR}  # Add generated shaders directory
)

# Linking
target_link_libraries(${PROJECT_NAME} PUBLIC
    GLFW 
    Glad 
    assimp 
    pugixml::pugixml 
    BulletDynamics 
    BulletCollision 
    LinearMath
    opengl32
    freetype
)

# Compile defs
target_compile_definitions(${PROJECT_NAME} PRIVATE
    CRT_SECURE_NO_WARNINGS
    PLATFORM_WINDOWS
    GLFW_INCLUDE_NONE
    STRIKE_USE_EMBEDDED_SHADERS  
    $<$<CONFIG:Debug>:STRIKE_DEBUG>
    $<$<CONFIG:Release>:STRIKE_RELEASE>
    $<$<CONFIG:Dist>:STRIKE_DIST>
)


# Export engine resources path for dependent projects
set(STRIKE_ENGINE_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources CACHE PATH "StrikeEngine resources directory")

# ============================================================================
# FUNCTION TO SETUP ENGINE FONTS FOR GAME PROJECTS
# ============================================================================
set(STRIKE_ENGINE_FONTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res/fonts CACHE PATH "StrikeEngine fonts directory")

function(strike_setup_fonts TARGET_NAME)
    if(WIN32)
        # Debug: Create junction link for fast development
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/fonts
            COMMAND $<$<CONFIG:Debug>:cmd> /c $<$<CONFIG:Debug>:"mklink /J \"$<TARGET_FILE_DIR:${TARGET_NAME}>/fonts\" \"${STRIKE_ENGINE_FONTS_DIR}\"">
            COMMENT "Setting up engine fonts link for Debug configuration"
        )
    else()
        # Debug: Create symlink 
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove $<TARGET_FILE_DIR:${TARGET_NAME}>/fonts
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E create_symlink ${STRIKE_ENGINE_FONTS_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/fonts
            COMMENT "Setting up engine fonts link for Debug"
        )
    endif()

    # Release: Copy fonts
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E remove_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/fonts
        COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E copy_directory ${STRIKE_ENGINE_FONTS_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/fonts
        COMMENT "Copying engine fonts for Release/Dist configuration"
    )
endfunction()