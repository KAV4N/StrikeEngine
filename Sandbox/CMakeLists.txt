cmake_minimum_required(VERSION 3.16)
project(Sandbox)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.h")
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE StrikeEngine)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLATFORM_WINDOWS
    $<$<CONFIG:Debug>:STRIKE_DEBUG>
    $<$<CONFIG:Release>:STRIKE_RELEASE>
    $<$<CONFIG:Dist>:STRIKE_DIST>
)

# Output executables in per-config folders
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/Sandbox/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Sandbox/Release"
    RUNTIME_OUTPUT_DIRECTORY_DIST    "${CMAKE_BINARY_DIR}/Sandbox/Dist"
)

# Setup StrikeEngine resources using the function from StrikeEngine
strike_engine_setup_resources(${PROJECT_NAME})

# Setup Sandbox-specific assets
if(WIN32)
    # Debug: Create junction link for fast development
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
        COMMAND $<$<CONFIG:Debug>:cmd> /c $<$<CONFIG:Debug>:"mklink /J \"$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets\" \"${CMAKE_SOURCE_DIR}/Sandbox/Assets\"">
        COMMENT "Setting up Sandbox assets for Debug configuration"
    )
else()
    # Debug: Create symlink for fast development
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
        COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E create_symlink ${CMAKE_SOURCE_DIR}/Sandbox/Assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
        COMMENT "Setting up Sandbox assets for Debug configuration"
    )
endif()

# Release/Dist: Copy assets for distribution
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
    COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E copy_directory ${CMAKE_SOURCE_DIR}/Sandbox/Assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
    COMMENT "Copying Sandbox assets for Release/Dist configuration"
)