# Getting Started

## Requirements

- **CMake 3.16 or higher**
- **C++17 compiler** - GCC or MSVC
- **Windows** - primary platform (Linux may require adjustments)
- **OpenGL 4.3-capable GPU**
- **Git** - required for cloning with submodules

---

## Setup

### 1. Clone StrikeEngine

```bash
git clone https://github.com/KAV4N/StrikeEngine.git
```
---

### 2. Directory Structure

Place your game project as a **sibling directory** next to the cloned `StrikeEngine` folder:

```

MyGame/                     
├── StrikeEngine/           ← cloned engine repo 
├── assets/
│   ├── Scenes/
│   │   └── MainScene.xml
│   ├── Models/
│   ├── Textures/
│   └── Sounds/
├── src/
│   ├── main.cpp
│   └── Scripts/
│       ├── PlayerController.h
│       ├── PlayerController.cpp
│       └── ...
├── CMakeLists.txt
└── CMakePresets.json
```
---

### 3. CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wreorder
        -Wno-unknown-pragmas
        -Wno-deprecated-copy
        -Wno-unused-parameter
    )
endif()

# Optional: display collision wireframes (experimental, may cause issues)
option(STRIKE_COLLISION_DEBUG_EXPERIMENTAL "Collision debug" OFF)

# StrikeEngine must be a sibling directory to your project
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../StrikeEngine/StrikeEngine
    ${CMAKE_CURRENT_BINARY_DIR}/StrikeEngine
)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE StrikeEngine)

# Place executable inside build/MyGame/
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}"
)

strike_setup(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/assets")
```

> `strike_setup` automatically creates junction links (Windows Debug), symlinks (Linux/macOS Debug), or full directory copies (Release/Dist) of both the engine's internal resources and your assets folder next to your executable.

---

### 4. CMakePresets.json

The presets file is **yours to own** - the engine's presets live inside the `StrikeEngine/` repo and are not inherited by your project. Create your own at the root of your game folder.

The `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` paths below point to a MinGW/MSYS2 GCC installation - adjust them to match your own toolchain location:

```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "windows-base",
            "hidden": true,
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/out/build/${presetName}",
            "installDir": "${sourceDir}/out/install/${presetName}",
            "cacheVariables": {
                "CMAKE_C_COMPILER": "C:/msys64/ucrt64/bin/gcc.exe",
                "CMAKE_CXX_COMPILER": "C:/msys64/ucrt64/bin/g++.exe"
            },
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Windows"
            }
        },
        {
            "name": "x64-debug",
            "displayName": "x64 Debug",
            "inherits": "windows-base",
            "architecture": {
                "value": "x64",
                "strategy": "external"
            },
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            }
        },
        {
            "name": "x64-release",
            "displayName": "x64 Release",
            "inherits": "x64-debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release"
            }
        },
    ]
}
```

> If you are using MSVC instead of MinGW, remove the `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` entries entirely and let CMake find MSVC automatically via a Developer Command Prompt.

---

### 5. Build

```bash
cd MyGame

# Configure
cmake --preset x64-debug

# Build
cmake --build --preset x64-debug
```

The executable and all required resources will be placed in `out/build/x64-debug/MyGame/`.

---

## Minimal Application

StrikeEngine exposes its entire public API through a single header.

```cpp
#include <StrikeEngine.h>

int main(int argc, char** argv) {
    Strike::Application app;
    app.run();
    return 0;
}
```

This creates a window and starts the game loop. Rendering, physics, audio, and script updates all run automatically.

> **Note:** All paths shown in the docs are for illustrative purposes and do not represent a required project structure.

---

## Next Step

- [Scene Format](02-scene-format.md)