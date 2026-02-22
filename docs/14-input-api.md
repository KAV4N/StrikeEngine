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

---

## Cursor Mode

#### `static void setCursorMode(CursorMode mode)`
Sets the cursor display and behavior mode.

```cpp
enum class CursorMode {
    Normal,  // Visible cursor, free to move
    Hidden,  // Cursor hidden but not locked
    Locked   // Cursor hidden and locked to window center
};
```

`Locked` is typically used for FPS-style camera controls where raw mouse delta is needed rather than absolute cursor position.

```cpp
// Lock cursor for FPS camera
Strike::Input::setCursorMode(Strike::CursorMode::Locked);

// Release cursor for UI
Strike::Input::setCursorMode(Strike::CursorMode::Normal);
```

---

## Notes

**Polling vs Events** - `Input` is polling-based, meaning you query state each frame. Use it in `onUpdate()` for continuous actions like movement or held buttons. For discrete actions like a single key press or release, prefer the event system via `onEvent()`.

**Static Access** - All methods are static. Use them directly without any instance:

```cpp
Strike::Input::isKeyPressed(STRIKE_KEY_A);
Strike::Input::getMouseXY();
Strike::Input::setCursorMode(Strike::CursorMode::Locked);
```

**Key and Mouse Codes** - Always use `STRIKE_KEY_*` and `STRIKE_MOUSE_BUTTON_*` macros instead of raw integers. See the [Event API](event-api.md) for the full reference tables.

## Next Step

- [Game Data](15-gameData-api.md)