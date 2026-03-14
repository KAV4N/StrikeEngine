# StrikeEngine Input API Documentation

## Overview

The `Input` class is a static utility class that provides polling-based access to keyboard and mouse input state. All methods are static - no instance is needed. Use it alongside the [Event System](EVENT_SYSTEM_API.md) for complete input handling: polling for continuous actions (held keys, cursor position), events for discrete actions (single press, release).
```cpp
// Example: polling in onUpdate()
if (Strike::Input::isKeyPressed(STRIKE_KEY_W)) {
    // move forward
}
```

---

## Keyboard Input

#### `static bool isKeyPressed(int keyCode)`
Returns `true` if the specified key is currently held down, `false` otherwise. Use `STRIKE_KEY_*` macros for key codes (see [Event System - Key Codes Reference](EVENT_SYSTEM_API.md#key-codes-reference)).
```cpp
if (Strike::Input::isKeyPressed(STRIKE_KEY_SPACE)) {
    // space is held
}
```

---

## Mouse Button Input

#### `static bool isMouseButtonPressed(int button)`
Returns `true` if the specified mouse button is currently held down, `false` otherwise. Use `STRIKE_MOUSE_BUTTON_*` macros for button codes (see [Event System - Mouse Button Codes Reference](EVENT_SYSTEM_API.md#mouse-button-codes-reference)).
```cpp
if (Strike::Input::isMouseButtonPressed(STRIKE_MOUSE_BUTTON_LEFT)) {
    // left button held
}
```

---

## Mouse Position

#### `static float getMouseX()`
Returns the current mouse cursor X position in pixels.

#### `static float getMouseY()`
Returns the current mouse cursor Y position in pixels.

#### `static std::pair<float, float> getMouseXY()`
Returns both X and Y positions in a single call. Prefer this over calling `getMouseX()` and `getMouseY()` separately to ensure both values are from the same moment.
```cpp
auto [x, y] = Strike::Input::getMouseXY();
```

> **Note:** When the cursor is `Locked`, these return an unbounded virtual position that accumulates indefinitely. Use `getMouseDelta()` for camera rotation instead.

---

## Mouse Delta

#### `static std::pair<float, float> getMouseDelta()`
Returns the mouse movement since the last frame as a `(deltaX, deltaY)` pair. Returns `(0, 0)` if the mouse did not move. This is the correct method to use for FPS-style camera rotation when the cursor is locked.
```cpp
auto [dX, dY] = Strike::Input::getMouseDelta();

// Yaw: rotate player body left/right
float yawDelta = -dX * mSensitivity;

// Pitch: rotate camera up/down
mPitch = glm::clamp(mPitch + (-dY * mSensitivity), -89.0f, 89.0f);
```

Delta is computed from raw mouse input when the platform supports it (no OS acceleration), falling back to processed input otherwise. The engine resets delta state automatically when switching to `Locked` mode, so no manual first-frame handling is required.

#### `static void update()`
Updates internal delta state. **Must be called once per frame** before any system reads mouse delta - typically at the top of your application's main loop. This is handled automatically if you are using `Strike::Application`; only call it manually if you are driving the loop yourself.
```cpp
// In your custom loop:
Strike::Input::update();
```

---

## Cursor Mode

#### `static void setCursorMode(CursorMode mode)`
Sets the cursor display and behavior mode.
```cpp
enum class CursorMode {
    Normal,  // Visible cursor, free to move
    Hidden,  // Cursor hidden but not locked
    Locked   // Cursor hidden and locked; raw input enabled if platform supports it
};
```

`Locked` is the correct mode for FPS-style camera controls. It also enables raw mouse motion on supported platforms, which removes OS pointer acceleration and gives 1:1 physical movement. Switching back to `Normal` or `Hidden` disables raw input automatically.

Calling `setCursorMode(Locked)` resets the internal delta state, so the first frame after locking will always produce a `(0, 0)` delta rather than a jump.
```cpp
// Lock cursor for FPS camera
Strike::Input::setCursorMode(Strike::CursorMode::Locked);

// Release cursor for UI
Strike::Input::setCursorMode(Strike::CursorMode::Normal);
```

---

## Notes

**Polling vs Events** - `Input` is polling-based, meaning you query state each frame. Use it in `onUpdate()` for continuous actions like movement or held buttons. For discrete actions like a single key press or release, prefer the event system via `onEvent()`.

**Camera rotation** - always use `getMouseDelta()` rather than tracking position manually. The engine handles first-frame initialisation, raw input opt-in, and delta reset on cursor lock so your script does not have to.

**Static Access** - All methods are static. Use them directly without any instance:
```cpp
Strike::Input::isKeyPressed(STRIKE_KEY_A);
Strike::Input::getMouseXY();
Strike::Input::getMouseDelta();
Strike::Input::setCursorMode(Strike::CursorMode::Locked);
```

**Key and Mouse Codes** - Always use `STRIKE_KEY_*` and `STRIKE_MOUSE_BUTTON_*` macros instead of raw integers. See the [Event API](event-api.md) for the full reference tables.

## Next Step

- [Game Data](15-gameData-api.md)