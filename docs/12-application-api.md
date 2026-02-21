# StrikeEngine Application & Window API Documentation

---

## Application

The `Application` class is the entry point and central lifecycle manager for a StrikeEngine application. It follows the singleton pattern, so only one instance can exist at a time.

### Singleton Access

#### `static Application& get()`
Returns a reference to the singleton Application instance.

### Application Lifecycle Control

#### `Application()`
Constructor that initializes the application, creates the window, and initializes renderers.

#### `virtual ~Application()`
Destructor for cleanup.

#### `void run()`
Runs the main application loop with delta time calculation and FPS limiting.

### Window Management

#### `Window& getWindow()`
Returns a reference to the Window object for window operations. See the [Window](#window) section below for all available methods.

### FPS Tracking and Control

#### `void setTargetFPS(int fps)`
Sets the target frames per second. Pass `0` for unlimited FPS.

#### `int getTargetFPS() const`
Returns the current target FPS setting.

#### `float getCurrentFPS() const`
Returns the actual current frames per second being achieved.

### Audio

#### `float getMasterVolume() const`
Returns the current global master volume.

#### `void setMasterVolume(float volume)`
Sets the global master volume. Accepted range is `0.0` (silent) to `1.0` (full volume).

```cpp
Application::get().setMasterVolume(0.5f); // set to 50%
float vol = Application::get().getMasterVolume();
```

### Notes

**Singleton Pattern** - The `Application` class follows the singleton pattern, so only one instance can exist at a time.

**Event Handling** - Event handling is managed internally through private methods (`onEvent`, `onWindowClose`, `onWindowResize`) and is not directly exposed as public API. Events are automatically routed from the Window to the Application and then forwarded to the World system.

**Main Loop** - The main application loop in `run()` calculates delta time, tracks FPS, and optionally limits frame rate based on the target FPS setting. Window close and resize events are handled automatically by the Application class.

---

## Window

The `Window` class is owned by `Application` and is accessed via `Application::getWindow()`. It provides control over the application window properties.

```cpp
Window& window = Application::get().getWindow();
```

### Public Methods

#### `unsigned int getWidth() const`
Returns the current window width in pixels.

#### `unsigned int getHeight() const`
Returns the current window height in pixels.

#### `void setVSync(bool enabled)`
Enable or disable VSync (vertical synchronization).

#### `bool isVSync() const`
Check if VSync is currently enabled.

#### `void setWindowTitle(const std::string& title)`
Set the window title.

#### `double getTime() const`
Get the current time in seconds since GLFW initialization.

### Usage Example

```cpp
Window& window = Application::get().getWindow();

unsigned int width  = window.getWidth();
unsigned int height = window.getHeight();

window.setVSync(true);
window.setWindowTitle("My Game");

double elapsed = window.getTime();
```

### Notes

**PIMPL Idiom** - The Window class uses the PIMPL (Pointer to Implementation) idiom with a private `WindowImpl` struct, which hides GLFW implementation details from users.

**Restricted Methods** - Private methods like `getNativeWindow()`, `setEventCallback()`, `onUpdate()`, `init()`, and `shutdown()` are not accessible to users. They are reserved for use by the `Application` and `Input` classes only.

**Ownership** - The Window is owned by the Application as a `std::unique_ptr<Window>` member variable and must always be accessed through `Application::getWindow()`.


## Next Step

- [Event API](13-event-api.md)