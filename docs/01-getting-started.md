# Getting Started

## Requirements

- **CMake 3.16 or higher**
- **C++17 compiler** - GCC or MSVC
- **Windows** - primary platform (Linux may require adjustments)
- **OpenGL 4.3-capable GPU**

---


## Creating Your Own Project

### Directory Structure

```
MyGame/
├── CMakeLists.txt
├── CMakePresets.json
├── src/
│   ├── main.cpp
│   └── Scripts/
│       ├── PlayerController.h
│       ├── PlayerController.cpp
│       └── ...
├── Assets/
│   ├── Scenes/
│   │   └── MainScene.xml
│   ├── Models/
│   ├── Textures/
│   └── Sounds/
└── StrikeEngine/
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)

# Optional: display collision wireframes (experimental, may cause issues)
option(STRIKE_COLLISION_DEBUG_EXPERIMENTAL "Collision debug" OFF)

add_subdirectory(StrikeEngine)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.h")
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE StrikeEngine)

strike_setup(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
```

> `strike_setup` automatically creates symlinks (Debug) or copies (Release) the engine's internal resource files into your build directory.

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

> **Note:** All paths displayed in the docs are only for pure usage example and do not showcase actual project structure.


---

## Next Step

- [Scene Format](02-scene-format.md) 
