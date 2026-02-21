# Scripting API

Scripts are how you add gameplay logic to entities in StrikeEngine. A script is a C++ class that inherits from `Strike::Script`, overrides lifecycle methods, and is registered with the engine so it can be instantiated by name from XML scene files.

---

## Quick Start

```cpp
// MyScript.h
#pragma once
#include "StrikeEngine.h"

class MyScript : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;
};
```

```cpp
// MyScript.cpp
#include "MyScript.h"

void MyScript::onStart() {
    // runs once, after all entities are initialized
}

void MyScript::onUpdate(float deltaTime) {
    // runs every frame
}

REGISTER_SCRIPT(MyScript)
```

```xml
<!-- Scene XML -->
<logic>
  <script type="MyScript"/>
</logic>
```

---

## Lifecycle Methods

Override any of these in your script class. All have empty default implementations.

| Method | When it runs |
|---|---|
| `onCreate()` | Once, immediately when the entity is assigned to the script |
| `onStart()` | Once, on the first frame update after `onCreate()` |
| `onUpdate(float deltaTime)` | Every frame while the script is active |
| `onDestroy()` | Once, when the script is destroyed (destructor) |
| `onEvent(Event& event)` | When an input or system event is dispatched |

**Execution order per frame:**

```
setEntity() → onCreate()          ← first time only
             → onStart()          ← first update only
             → onUpdate(dt)       ← every frame
```

**Recommended usage:**

- `onCreate()` - cache component references that don't depend on other entities
- `onStart()` - setup that requires other entities or systems to exist first
- `onUpdate()` - per-frame logic, input polling, movement
- `onEvent()` - discrete input events (single key press, mouse click)
- `onDestroy()` - cleanup (releasing resources, unsubscribing, etc.)

---

## Entity & Component Access

### Getting the Entity

```cpp
const Strike::Entity& entity = getEntity();
```

### Activity Control

```cpp
bool active = isActive();   // checks mActive on the script itself
setActive(false);           // deactivates the script itself
bool started = isStarted(); // true after onStart() has been called
```

`isActive()` and `setActive()` operate on the script's own `mActive` flag. `isStarted()` can be useful if you need to guard logic that should only run after `onStart()` has completed, for example in helper methods called from both `onUpdate()` and `onEvent()`.

### Component Methods

All component methods are templated on the component type:

```cpp
// Add
auto& rb = addComponent<Strike::PhysicsComponent>();

// Get (throws if not present) - mutable and const overloads available
auto& renderer = getComponent<Strike::RendererComponent>();
const auto& renderer = getComponent<Strike::RendererComponent>(); // from const context

// Check
if (hasComponent<Strike::LightComponent>()) { ... }

// Remove
removeComponent<Strike::LightComponent>();

// Get or add
auto& logic = getOrAddComponent<Strike::LogicComponent>();
```

### scriptEntity Alias

The protected member `scriptEntity` is a reference to the internal `mEntity` and can be used directly in subclass implementations as a shorthand:

```cpp
class MyScript : public Strike::Script {
    void onUpdate(float dt) override {
        scriptEntity.move(glm::vec3(0, 0, 1) * dt); // same as getEntity() but writable
    }
};
```

`getEntity()` returns a `const Entity&`. Use `scriptEntity` when you need to call non-const methods on the entity directly from within a subclass.

---

## Timer Utility

`tick(float seconds)` returns `true` once per interval - useful for cooldowns and periodic logic without managing your own timer variables:

```cpp
void onUpdate(float deltaTime) override {
    if (tick(2.0f)) {
        // runs once every 2 seconds
    }
}
```

Each unique interval value is tracked independently, so you can have multiple independent timers in one script:

```cpp
void onUpdate(float deltaTime) override {
    if (tick(0.5f)) spawnParticle();
    if (tick(5.0f)) respawnEnemy();
}
```

Timers are stored internally as a `float`-keyed map, so the interval value itself is the key. Calling `tick()` with the same literal value always refers to the same timer.

---

## LogicComponent

Multiple scripts of **different types** can be attached to a single entity via `LogicComponent`. Each script type may only appear **once per entity** - adding the same script type more than once is not allowed and will log a `STRIKE_CORE_ERROR` at runtime.

> **Note:** Only one instance of each script type can exist on a given entity at a time. Attempting to add a duplicate type - either programmatically via `addScript<T>()` or through XML - will be rejected with an error log, and the existing instance will be returned instead. To replace a script, call `removeScript<T>()` first.

Scripts are managed by the `LogicComponent`. You can add and query scripts programmatically at runtime rather than through XML:

```cpp
auto& logic = entity.getComponent<Strike::LogicComponent>();

// Add - will log STRIKE_CORE_ERROR and return existing instance if type is already attached
MyScript* script = logic.addScript<MyScript>();

// Get (returns nullptr if not found)
MyScript* existing = logic.getScript<MyScript>();

// Check
if (logic.hasScript<MyScript>()) { ... }

// Remove the instance of a type
logic.removeScript<MyScript>();
```

**To replace a script of a given type:**

```cpp
logic.removeScript<MyScript>(); // remove existing instance first
logic.addScript<MyScript>();    // then add a fresh one
```

---

## REGISTER_SCRIPT Macro

Every script **must** be registered at the bottom of its `.cpp` file. Without this, the engine cannot instantiate the script from XML.

```cpp
REGISTER_SCRIPT(MyScript)
```

This registers a factory function under the class name as a string. The name used in XML `type="..."` must match the class name exactly.

Under the hood, the macro expands to a static initializer that calls `ScriptRegistry::registerScriptFactory()` at program startup.

---

## ScriptRegistry (Advanced)

The `ScriptRegistry` is the factory system that backs `REGISTER_SCRIPT`. You generally don't need to use it directly, but it can be useful for tooling or debugging:

```cpp
// Check if a script type is registered
bool exists = Strike::ScriptRegistry::hasScriptFactory("MyScript");

// Instantiate by name
auto script = Strike::ScriptRegistry::createScript("MyScript");
// returns std::unique_ptr<Script>, or nullptr if not registered

// List all registered scripts
auto names = Strike::ScriptRegistry::getRegisteredScripts();
```

---

## XML Integration

Attach scripts to entities in your scene file using the `<logic>` component:

```xml
<entity tag="Player">
  <components>
    <logic>
      <script type="PlayerController"/>
      <script type="HealthSystem"/>
    </logic>
  </components>
</entity>
```

Scripts are instantiated in the order they appear. The `type` value must exactly match the registered class name. Each `type` may only appear **once per `<logic>` block** - duplicate entries will be rejected with a `STRIKE_CORE_ERROR` and the second entry will be ignored.

---

## Camera Movement Example

```cpp
// CameraMovement.h
#pragma once
#include "StrikeEngine.h"

class CameraMovement : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    float mLastX = 0.0f;
    float mLastY = 0.0f;
    bool  mFirstMouse = true;
    float mSensitivity = 0.1f;
    bool  mRotationEnabled = true;
    bool  mPKeyWasPressed = false;
};
```

```cpp
// CameraMovement.cpp
#include "CameraMovement.h"

void CameraMovement::onStart()
{
    Strike::Input::setCursorMode(Strike::CursorMode::Locked);

    auto [x, y] = Strike::Input::getMouseXY();
    mLastX = x;
    mLastY = y;
}

void CameraMovement::onUpdate(float deltaTime)
{
    glm::vec3 move(0.0f);
    const float speed = 20.0f * deltaTime;

    if (Strike::Input::isKeyPressed(STRIKE_KEY_D)) move.x += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_A)) move.x -= speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_W)) move.z -= speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_S)) move.z += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_SPACE)) move.y += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT)) move.y -= speed;

    if (glm::length(move) > 0.0f) {
        scriptEntity.move(move);
    }

    // Toggle rotation and cursor mode on P press (edge detection)
    bool pPressed = Strike::Input::isKeyPressed(STRIKE_KEY_P);
    if (pPressed && !mPKeyWasPressed) {
        mRotationEnabled = !mRotationEnabled;

        if (mRotationEnabled) {
            Strike::Input::setCursorMode(Strike::CursorMode::Locked);
            mFirstMouse = true; // reset to avoid jump when re-enabling
        } else {
            Strike::Input::setCursorMode(Strike::CursorMode::Normal);
        }
    }
    mPKeyWasPressed = pPressed;

    if (!mRotationEnabled)
        return;

    auto [x, y] = Strike::Input::getMouseXY();

    if (mFirstMouse) {
        mLastX = x;
        mLastY = y;
        mFirstMouse = false;
    }

    float deltaX = x - mLastX;
    float deltaY = y - mLastY;

    mLastX = x;
    mLastY = y;

    glm::vec3 rotation(0.0f);
    rotation.y = -deltaX * mSensitivity;  // yaw
    rotation.x = -deltaY * mSensitivity;  // pitch

    if (glm::length(rotation) > 0.0f)
        scriptEntity.rotate(rotation);
}

REGISTER_SCRIPT(CameraMovement);
```

---

## Notes

### One Script Type Per Entity

Each script type can only be attached **once per entity**. Attempting to add a duplicate type - via `addScript<T>()` at runtime or via a duplicate `<script type="..."/>` entry in XML - will be rejected. A `STRIKE_CORE_ERROR` will be logged and the call will return the existing instance rather than creating a new one. Use `removeScript<T>()` before re-adding if a fresh instance is needed.

### Thread Safety

Scripts execute on the main thread as part of the `ScriptSystem` update cycle and are not thread-safe. Do not call script methods from other threads.

### Memory Management

Scripts are owned by `std::unique_ptr` inside `LogicComponent`. The engine handles their entire lifecycle - you do not need to manually allocate or delete them.

Scripts are non-copyable and non-movable by design. Copy and move constructors and assignment operators are all deleted. This ensures scripts are never accidentally sliced or aliased when stored inside the `LogicComponent`.


## Next Step

- [Entity API](05-entity-api.md)