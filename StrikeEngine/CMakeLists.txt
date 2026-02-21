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
)

# Platform-specific linking
target_link_libraries(${PROJECT_NAME} PUBLIC
    GLFW
    Glad
    assimp
    pugixml::pugixml
    BulletDynamics
    BulletCollision
    LinearMath
    freetype
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC opengl32)
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
endif()

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    CRT_SECURE_NO_WARNINGS
    GLFW_INCLUDE_NONE
    $<$<CONFIG:Debug>:STRIKE_DEBUG>
    $<$<CONFIG:Release>:STRIKE_RELEASE>
    $<$<CONFIG:Dist>:STRIKE_DIST>
    $<$<BOOL:${STRIKE_COLLISION_DEBUG_EXPERIMENTAL}>:STRIKE_COLLISION_DEBUG_EXPERIMENTAL>
)

# ============================================================================
# FUNCTION TO SETUP ENGINE RESOURCES AND GAME ASSETS
# TARGET_NAME  - the game executable target
# ASSETS_DIR   - the source directory where the user keeps their assets
# ============================================================================
set(STRIKE_ENGINE_RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res CACHE PATH "StrikeEngine res directory")

function(strike_setup TARGET_NAME ASSETS_DIR)
    get_filename_component(ASSETS_NAME "${ASSETS_DIR}" NAME)

    if(WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/res"
            COMMAND $<$<CONFIG:Debug>:cmd> /c $<$<CONFIG:Debug>:"mklink /J \"$<TARGET_FILE_DIR:${TARGET_NAME}>/res\" \"${STRIKE_ENGINE_RES_DIR}\"">
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/${ASSETS_NAME}"
            COMMAND $<$<CONFIG:Debug>:cmd> /c $<$<CONFIG:Debug>:"mklink /J \"$<TARGET_FILE_DIR:${TARGET_NAME}>/${ASSETS_NAME}\" \"${ASSETS_DIR}\"">
            COMMENT "Creating junction links for res and ${ASSETS_NAME} (Debug)"
        )
    else()
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove "$<TARGET_FILE_DIR:${TARGET_NAME}>/res"
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E create_symlink "${STRIKE_ENGINE_RES_DIR}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/res"
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove "$<TARGET_FILE_DIR:${TARGET_NAME}>/${ASSETS_NAME}"
            COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E create_symlink "${ASSETS_DIR}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/${ASSETS_NAME}"
            COMMENT "Creating symlinks for res and ${ASSETS_NAME} (Debug)"
        )
    endif()

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E remove_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/res"
        COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E copy_directory "${STRIKE_ENGINE_RES_DIR}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/res"
        COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E remove_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/${ASSETS_NAME}"
        COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E copy_directory "${ASSETS_DIR}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/${ASSETS_NAME}"
        COMMENT "Copying res and ${ASSETS_NAME} (Release/Dist)"
    )
endfunction()