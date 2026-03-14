# StrikeEngine Application & Window API Documentation


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
Sets the global master volume. Accepted range is `0.0` (silent) to `1.0`+.

#### `float getAudioAmplitude(const Entity& entity) const`
Returns the current RMS amplitude in the range `[0.0, 1.0]` of the first playing sound on the given entity. Reads a small PCM window from the audio source without affecting the playback cursor. Returns `0.0` if the entity has no active `AudioSourceComponent`, no sound is currently playing, or the audio system is unavailable.

```cpp
Application::get().setMasterVolume(0.5f); // set to 50%
float vol = Application::get().getMasterVolume();

float amp = Application::get().getAudioAmplitude(entity); // 0.0 – 1.0
```

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

#### `void setSize(unsigned int width, unsigned int height)`
Sets the window dimensions in pixels. Triggers a resize event after a short debounce delay.

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

window.setSize(1920, 1080);
window.setVSync(true);
window.setWindowTitle("My Game");

double elapsed = window.getTime();
```

---

## Next Step

- [Event API](13-event-api.md)