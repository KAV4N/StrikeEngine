cmake_minimum_required(VERSION 3.16)
project(Sandbox)

set(CMAKE_CXX_STANDARD 17)

# Define the executable target first
add_executable(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_SOURCES "*.cpp" "*.h")

# Add sources to the executable
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/src
    ${CMAKE_SOURCE_DIR}/StrikeEngine/src/scripts
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/glm
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/imgui
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/entt/single_include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/Glad/include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/json/include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/assimp/include
    ${CMAKE_SOURCE_DIR}/StrikeEngine/vendor/pugixml/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE StrikeEngine)

target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    $<$<CONFIG:Debug>:STRIKE_DEBUG>
    $<$<CONFIG:Release>:STRIKE_RELEASE>
    $<$<CONFIG:Dist>:STRIKE_DIST>
)

# Output executables to Sandbox folder
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Sandbox/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Sandbox/Release"
    RUNTIME_OUTPUT_DIRECTORY_DIST "${CMAKE_BINARY_DIR}/Sandbox/Dist"
)

# Function to create symbolic links (Debug mode only)
function(create_asset_symlinks target_name)
    if(WIN32)
        # On Windows, create directory junctions (symbolic links for directories)
        add_custom_command(TARGET ${target_name} POST_BUILD
            # Remove existing links/directories first
            COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${target_name}>/Assets
            # Create symbolic links (junctions on Windows)
            COMMAND cmd /c "mklink /J \"$<TARGET_FILE_DIR:${target_name}>/Assets\" \"${CMAKE_SOURCE_DIR}/Sandbox/Assets\""
            COMMENT "Creating symbolic links for Assets directories (Debug mode)"
        )
    else()
        # On Unix-like systems (Linux, macOS), create symbolic links
        add_custom_command(TARGET ${target_name} POST_BUILD
            # Remove existing links/directories first
            COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE_DIR:${target_name}>/Assets
            # Create symbolic links
            COMMAND ${CMAKE_COMMAND} -E create_symlink 
                ${CMAKE_SOURCE_DIR}/Sandbox/Assets 
                $<TARGET_FILE_DIR:${target_name}>/Assets
            COMMENT "Creating symbolic links for Assets and scenes directories (Debug mode)"
        )
    endif()
endfunction()

# Function to copy directories (Release/Dist modes)
function(copy_asset_directories target_name)
    add_custom_command(TARGET ${target_name} POST_BUILD
        # Remove existing directories first
        COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${target_name}>/Assets
        # Copy directories
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            ${CMAKE_SOURCE_DIR}/Sandbox/Assets 
            $<TARGET_FILE_DIR:${target_name}>/Assets
        COMMENT "Copying Assets and scenes directories (Release/Dist mode)"
    )
endfunction()

# Apply different asset handling based on build configuration
# Debug mode: Create symbolic links for faster development
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Setting up Assets for $<CONFIG> configuration..."
)

# Debug configuration - use symbolic links
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove_directory $<$<CONFIG:Debug>:$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets>
        COMMAND $<$<CONFIG:Debug>:cmd> /c $<$<CONFIG:Debug>:"mklink /J \"$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets\" \"${CMAKE_SOURCE_DIR}/Sandbox/Assets\"">
        COMMENT "Creating symbolic links for Debug configuration"
    )
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E remove $<$<CONFIG:Debug>:$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets>
        COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E create_symlink 
            $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Sandbox/Assets>
            $<$<CONFIG:Debug>:$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets>
        COMMENT "Creating symbolic links for Debug configuration"
    )
endif()

# Release and Dist configurations - copy files
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E remove_directory $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets>
    COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_COMMAND}> -E copy_directory 
        $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_SOURCE_DIR}/Sandbox/Assets>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets>
    COMMENT "Copying Assets and scenes for Release/Dist configuration"
)