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
    // runs once, before the first update
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
| `onStart()` | Once, on the first frame update after the entity is assigned |
| `onUpdate(float deltaTime)` | Every frame while the script is active |
| `onDestroy()` | Once, when the script is destroyed |
| `onEvent(Event& event)` | When an input or system event is dispatched |

**Execution order per frame:**
```
setEntity()  ← first time only
→ onStart()  ← first update only
→ onUpdate(dt) ← every frame
```

**Recommended usage:**

- `onStart()` - initialization, caching component references, setup that requires the entity to be valid
- `onUpdate()` - per-frame logic, input polling, movement
- `onEvent()` - discrete input events (single key press, mouse click)
- `onDestroy()` - cleanup (releasing resources, unsubscribing, etc.)

---

## Entity & Component Access

### Getting the Entity
```cpp
Strike::Entity entity = getEntity();
```

`getEntity()` returns a copy of the entity handle. Since `Entity` is a lightweight value type, copying it is cheap and safe.

### scriptEntity Alias

The protected member `scriptEntity` is a direct reference to the internal entity and is the preferred way to access it from within subclasses:
```cpp
class MyScript : public Strike::Script {
    void onUpdate(float dt) override {
        scriptEntity.move(glm::vec3(0, 0, 1) * dt);
        scriptEntity.getComponent<Strike::RendererComponent>();
        scriptEntity.addComponent<Strike::LightComponent>();
    }
};
```

All component operations (`getComponent`, `addComponent`, `hasComponent`, `removeComponent`, `getOrAddComponent`) are available directly on `scriptEntity` or on the copy returned by `getEntity()`.

### Activity Control
```cpp
bool active = isActive();    // checks the script's own active flag
setActive(false);            // deactivates this script
bool started = isStarted();  // true after onStart() has been called
```

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

Timers are stored as a `float`-keyed map, so the interval value itself is the key. Calling `tick()` with the same literal always refers to the same timer.

---

## LogicComponent

Multiple scripts of **different types** can be attached to a single entity via `LogicComponent`. Each script type may only appear **once per entity**.

> **Note:** Attempting to add a duplicate type - via `addScript<T>()` or through XML - will be rejected with a warning, and a reference to the existing instance will be returned. Use `removeScript<T>()` first to replace a script.

```cpp
auto& logic = entity.getComponent<Strike::LogicComponent>();

auto& script = logic.addScript<MyScript>();       // add
MyScript* existing = logic.getScript<MyScript>(); // get (nullptr if not found)
if (logic.hasScript<MyScript>()) { ... }          // check
logic.removeScript<MyScript>();                   // remove
```

**To replace a script:**
```cpp
logic.removeScript<MyScript>();
logic.addScript<MyScript>();
```

---

## REGISTER_SCRIPT Macro

Every script **must** be registered at the bottom of its `.cpp` file:
```cpp
REGISTER_SCRIPT(MyScript)
```

Without this, the engine cannot instantiate the script from XML and any call to `addScript<T>()` will trigger an assertion. The `type="..."` value in XML must match the class name exactly.

---

## ScriptRegistry (Advanced)

```cpp
bool exists = Strike::ScriptRegistry::hasScriptFactory("MyScript");
auto script = Strike::ScriptRegistry::createScript("MyScript"); // unique_ptr<Script> or nullptr
auto names  = Strike::ScriptRegistry::getRegisteredScripts();
```

---

## XML Integration

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

Scripts are instantiated in order. Each `type` may only appear once per `<logic>` block - duplicates are rejected with a warning. Unknown types are skipped with an error log.

---

## Camera Movement Example
```cpp
// CameraMovement.h
#pragma once
#include "StrikeEngine.h"

class CameraMovement : public Strike::Script {
public:
    void onUpdate(float deltaTime) override;

private:
    float mMoveSpeed = 20.0f;
    float mLookSpeed = 90.0f;
};
```
```cpp
// CameraMovement.cpp
#include "CameraMovement.h"

void CameraMovement::onUpdate(float deltaTime) {
    glm::vec3 move(0.0f);
    float speed = mMoveSpeed * deltaTime;

    if (Strike::Input::isKeyPressed(STRIKE_KEY_W))          move.z -= speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_S))          move.z += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_A))          move.x -= speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_D))          move.x += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_SPACE))      move.y += speed;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_LEFT_SHIFT)) move.y -= speed;

    if (glm::length(move) > 0.0f)
        scriptEntity.move(move);

    float look = mLookSpeed * deltaTime;
    glm::vec3 euler(0);

    if (Strike::Input::isKeyPressed(STRIKE_KEY_LEFT))  euler.y += look;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_RIGHT)) euler.y -= look;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_UP))    euler.x += look;
    if (Strike::Input::isKeyPressed(STRIKE_KEY_DOWN))  euler.x -= look;

    scriptEntity.rotate(euler);
}

REGISTER_SCRIPT(CameraMovement)
```

---

## Notes

### Memory Management
Scripts are owned by `std::unique_ptr` inside `LogicComponent`. The engine handles their entire lifecycle - you do not need to manually allocate or delete them. Scripts are non-copyable and non-movable by design.

### addScript Always Returns a Valid Reference
`addScript<T>()` always returns a valid reference - either a new script or the already-attached one. It asserts at startup if the type was never registered with `REGISTER_SCRIPT`.

---

## Next Step

- [Entity API](05-entity-api.md)