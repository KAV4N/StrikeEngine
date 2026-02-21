# StrikeEngine Event System API Documentation

## Overview

The event system in StrikeEngine handles input and application events by routing them from GLFW through the Window, Application, World, and ScriptSystem layers down to individual Scripts.

---

## Event Flow

```
GLFW → Window → Application → World → ScriptSystem → Scripts
```

Each layer can stop further propagation by setting `event.handled = true`.

---

## Base Event Class

The `Event` base class is abstract and serves as the foundation for all events in StrikeEngine.

```cpp
class Event {
public:
    virtual EventType getEventType() const = 0; // Returns the event type enum value
    virtual const char* getName() const = 0;    // Returns the event name as a C-string
    virtual std::string toString() const;       // Returns string representation (default: getName())

    bool handled = false; // Set to true to stop further propagation
};
```

---

## EventType Enum

Defines all possible event types in the system:

```cpp
enum class EventType {
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};
```

---

## EventDispatcher

The `EventDispatcher` class provides type-safe event routing. Construct it with an event reference, then call `dispatch<T>()` with a handler function. The handler is only called if the event type matches `T`.

```cpp
EventDispatcher dispatcher(event);
dispatcher.dispatch<KeyPressedEvent>([](KeyPressedEvent& e) -> bool {
    // handle event
    return true; // return true to mark as handled
});
```

#### `template<typename T> bool dispatch(EventFn<T> func)`
Dispatches the event to `func` if the event type matches `T`. Sets `event.handled` to the return value of `func`. Returns true if dispatched, false if the type did not match.

---

## Application Events

### `WindowResizeEvent`

Fired when the window is resized. Window resize events are **throttled with a 150ms delay** to prevent excessive rendering updates.

```cpp
unsigned int getWidth() const;   // New window width in pixels
unsigned int getHeight() const;  // New window height in pixels
```

### `WindowCloseEvent`

Fired when the window is closed. Has no additional data.

---

## Key Events

### `KeyEvent` (Base Class)

```cpp
int getKeyCode() const; // GLFW key code (use STRIKE_KEY_* macros)
```

### `KeyPressedEvent`

Fired on key press and key repeat.

```cpp
int getKeyCode() const;      // Key code
int getRepeatCount() const;  // 0 for initial press, 1 for repeat
```

### `KeyReleasedEvent`

Fired when a key is released.

```cpp
int getKeyCode() const; // Key code
```

### `KeyTypedEvent`

Fired for character input (e.g. text fields). Provides the Unicode character code.

```cpp
int getKeyCode() const; // Unicode character code
```

---

## Mouse Events

### `MouseMovedEvent`

Fired when the mouse cursor moves.

```cpp
float getX() const; // Mouse X position in pixels
float getY() const; // Mouse Y position in pixels
```

### `MouseScrolledEvent`

Fired when the mouse wheel is scrolled.

```cpp
float getXOffset() const; // Horizontal scroll offset
float getYOffset() const; // Vertical scroll offset
```

### `MouseButtonEvent` (Base Class)

```cpp
int getMouseButton() const; // Mouse button code (use STRIKE_MOUSE_BUTTON_* macros)
```

### `MouseButtonPressedEvent`

Fired when a mouse button is pressed.

```cpp
int getMouseButton() const; // Mouse button code
```

### `MouseButtonReleasedEvent`

Fired when a mouse button is released.

```cpp
int getMouseButton() const; // Mouse button code
```

---

## Script Event Handling

Override `onEvent()` in your script class to receive events:

```cpp
class MyScript : public Strike::Script {
public:
    void onEvent(Strike::Event& event) override;
};
```

### Usage Pattern

```cpp
void MyScript::onEvent(Strike::Event& event) {
    if (event.getEventType() == Strike::EventType::MouseButtonPressed) {
        auto& mouseEvent = static_cast<Strike::MouseButtonPressedEvent&>(event);

        if (mouseEvent.getMouseButton() == STRIKE_MOUSE_BUTTON_LEFT) {
            // handle left click
        } else if (mouseEvent.getMouseButton() == STRIKE_MOUSE_BUTTON_RIGHT) {
            // handle right click
        }

        event.handled = true; // stop further propagation
    }
}
```

Scripts only receive events if they are **active** and have been **started** (`isStarted() == true`). Events are dispatched to scripts in entity order and stop as soon as one sets `event.handled = true`.

---

## Key Codes Reference

Use `STRIKE_KEY_*` macros for keyboard input:

```cpp
// Letters
STRIKE_KEY_A through STRIKE_KEY_Z

// Numbers
STRIKE_KEY_0 through STRIKE_KEY_9

// Function Keys
STRIKE_KEY_F1 through STRIKE_KEY_F25

// Special Keys
STRIKE_KEY_SPACE          // 32
STRIKE_KEY_ESCAPE         // 256
STRIKE_KEY_ENTER          // 257
STRIKE_KEY_TAB            // 258
STRIKE_KEY_BACKSPACE      // 259
STRIKE_KEY_INSERT         // 260
STRIKE_KEY_DELETE         // 261
STRIKE_KEY_RIGHT          // 262
STRIKE_KEY_LEFT           // 263
STRIKE_KEY_DOWN           // 264
STRIKE_KEY_UP             // 265
STRIKE_KEY_PAGE_UP        // 266
STRIKE_KEY_PAGE_DOWN      // 267
STRIKE_KEY_HOME           // 268
STRIKE_KEY_END            // 269
STRIKE_KEY_CAPS_LOCK      // 280
STRIKE_KEY_LEFT_SHIFT     // 340
STRIKE_KEY_LEFT_CONTROL   // 341
STRIKE_KEY_LEFT_ALT       // 342
STRIKE_KEY_RIGHT_SHIFT    // 344
STRIKE_KEY_RIGHT_CONTROL  // 345
STRIKE_KEY_RIGHT_ALT      // 346

// Numpad
STRIKE_KEY_KP_0 through STRIKE_KEY_KP_9
STRIKE_KEY_KP_DECIMAL     // 330
STRIKE_KEY_KP_DIVIDE      // 331
STRIKE_KEY_KP_MULTIPLY    // 332
STRIKE_KEY_KP_SUBTRACT    // 333
STRIKE_KEY_KP_ADD         // 334
STRIKE_KEY_KP_ENTER       // 335
STRIKE_KEY_KP_EQUAL       // 336
```

## Mouse Button Codes Reference

Use `STRIKE_MOUSE_BUTTON_*` macros for mouse input:

```cpp
STRIKE_MOUSE_BUTTON_LEFT    // 0
STRIKE_MOUSE_BUTTON_RIGHT   // 1
STRIKE_MOUSE_BUTTON_MIDDLE  // 2
STRIKE_MOUSE_BUTTON_1       // 0
STRIKE_MOUSE_BUTTON_2       // 1
STRIKE_MOUSE_BUTTON_3       // 2
STRIKE_MOUSE_BUTTON_4       // 3
STRIKE_MOUSE_BUTTON_5       // 4
STRIKE_MOUSE_BUTTON_6       // 5
STRIKE_MOUSE_BUTTON_7       // 6
STRIKE_MOUSE_BUTTON_8       // 7  (STRIKE_MOUSE_BUTTON_LAST)
```

---

## Notes

**Event Propagation** - Events flow from GLFW → Window → Application → World → ScriptSystem → Scripts. Each layer can stop propagation by setting `event.handled = true`.

**Type Safety** - The `EventDispatcher::dispatch<T>()` template method ensures type safety by checking the event type before casting.

**Window Resize Throttling** - Window resize events are throttled with a 150ms delay to prevent excessive rendering updates during resize drags.

**Script Dispatch Order** - Scripts receive events in entity order. The first script to set `event.handled = true` stops all further dispatch for that event.

**`EVENT_CLASS_TYPE` Macro** - Used internally by all event classes to implement `getEventType()`, `getStaticType()`, and `getName()` without boilerplate. You do not need to use this macro in script code.

## Next Step

- [Input API](14-input-api.md)