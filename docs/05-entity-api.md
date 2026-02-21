# Entity API

The `Entity` class is a lightweight handle to an object in the scene. It wraps an EnTT entity handle and delegates operations to the scene's ECS registry (for components) and an internal `GraphNode` (for transforms and hierarchy). Entities are copyable and movable. You can use `scriptEntity` protected reference to entity or `getEntity()` method to retrieve entity that owns the script.

---

## Quick Reference

```cpp
void MyScript::onStart() {
    // Get another entity from the scene by tag
    auto player = getEntity().getScene()->getEntity("Player");

    // Check validity before using
    if (player.isValid()) {
        player.setPosition(glm::vec3(0, 1, 0));
    }

    // Bool conversion works the same as isValid()
    if (player) { ... }
}
```

---

## Construction & Validity

Entities are created and managed by the scene. You don't construct them directly - you receive them from `getEntity()`, `getScene()->getEntity()`, or `getScene()->createEntity()`.

```cpp
// Default-constructed entity - invalid, use only as a null placeholder
Strike::Entity empty;
empty.isValid(); // false
```

Always check validity before using an entity obtained from a scene query:

```cpp
void MyScript::onStart() {
    auto target = getEntity().getScene()->getEntity("Target");

    if (target.isValid()) {
        // safe to use
    }

    // Equivalent bool check
    if (target) { ... }
}
```

---

## Active State

```cpp
void MyScript::onUpdate(float deltaTime) {
    auto enemy = getEntity().getScene()->getEntity("Enemy");

    if (enemy.isActive()) {
        // enemy is participating in updates and rendering
    }

    // Deactivate - the entity is skipped by most systems
    enemy.setActive(false);
}
```

---

## Tag

Tags are string identifiers used to look up entities by name in the scene.

```cpp
void MyScript::onCreate() {
    // Rename this entity
    scriptEntity.setTag("PlayerController");

    // Read the tag back
    const std::string& name = scriptEntity.getTag();
}
```

---

## Component Management

All component methods are templated on the component type. When writing scripts, prefer the inherited shorthand methods (`addComponent<T>()`, `getComponent<T>()`, etc.) over calling them through `getEntity()` - they resolve to the same thing with less typing.

```cpp
void MyScript::onCreate() {
    // Add a component
    auto& light = addComponent<Strike::LightComponent>();

    // Get - mutable
    auto& renderer = getComponent<Strike::RendererComponent>();

    // Get - const (safe in const methods)
    const auto& constRenderer = getComponent<Strike::RendererComponent>();

    // Check before getting to avoid throws
    if (hasComponent<Strike::PhysicsComponent>()) {
        auto& physics = getComponent<Strike::PhysicsComponent>();
    }

    // Remove
    removeComponent<Strike::LightComponent>();

    // Get existing or add if missing
    auto& logic = getOrAddComponent<Strike::LogicComponent>();
}

void MyScript::onUpdate(float deltaTime) {
    // Access a component on a different entity
    auto lamp = getEntity().getScene()->getEntity("Lamp");
    if (lamp && lamp.hasComponent<Strike::LightComponent>()) {
        lamp.getComponent<Strike::LightComponent>().setIntensity(2.0f);
    }
}
```

---

## Local Transform

Local transforms are relative to the entity's parent. For root entities, local and world transforms are equivalent.

> All Euler angles are in **degrees**, not radians.

### Position

```cpp
void MyScript::onUpdate(float deltaTime) {
    // Read local position
    const glm::vec3& pos = scriptEntity.getPosition();

    // Set local position
    scriptEntity.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

    // Move in the direction the entity is currently facing
    scriptEntity.move(glm::vec3(0.0f, 0.0f, 1.0f * deltaTime));
}
```

`move()` applies the delta in heading space, so it respects the entity's current orientation - moving along Z always means "forward," regardless of which way the entity is rotated.

### Rotation

```cpp
void MyScript::onUpdate(float deltaTime) {
    // Read as quaternion
    const glm::quat& rot = scriptEntity.getRotation();

    // Set as quaternion
    scriptEntity.setRotation(glm::quat(glm::vec3(0, glm::radians(90.0f), 0)));

    // Read as Euler angles (degrees)
    glm::vec3 angles = scriptEntity.getEulerAngles();

    // Set as Euler angles (degrees)
    scriptEntity.setEulerAngles(glm::vec3(0.0f, 90.0f, 0.0f));

    // Add to current rotation
    scriptEntity.rotate(glm::vec3(0.0f, 45.0f * deltaTime, 0.0f));

    // Rotate around a world axis
    scriptEntity.rotateQuaternion(90.0f * deltaTime, glm::vec3(0, 1, 0));
}
```

### Scale

```cpp
void MyScript::onStart() {
    const glm::vec3& scale = scriptEntity.getScale();
    scriptEntity.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
}
```

---

## World Transform

World transforms are absolute - they account for the full parent chain. Use these when you need to read or set a position in global space regardless of the hierarchy.

```cpp
void MyScript::onUpdate(float deltaTime) {
    // World position
    glm::vec3 worldPos = scriptEntity.getWorldPosition();
    scriptEntity.setWorldPosition(glm::vec3(10.0f, 0.0f, 5.0f));

    // World rotation
    glm::quat worldRot = scriptEntity.getWorldRotation();
    scriptEntity.setWorldRotation(worldRot);

    glm::vec3 worldAngles = scriptEntity.getWorldEulerAngles();
    scriptEntity.setWorldEulerAngles(glm::vec3(0.0f, 180.0f, 0.0f));

    // World scale - read only
    glm::vec3 worldScale = scriptEntity.getWorldScale();
}
```

---

## Direction Vectors

Derived from the entity's world rotation. Useful for movement and aiming.

```cpp
void MyScript::onUpdate(float deltaTime) {
    glm::vec3 forward = scriptEntity.getForward(); // -Z axis
    glm::vec3 right   = scriptEntity.getRight();   // +X axis
    glm::vec3 up      = scriptEntity.getUp();      // +Y axis

    // Move forward in world space using direction vector
    glm::vec3 newPos = scriptEntity.getWorldPosition() + forward * 5.0f * deltaTime;
    scriptEntity.setWorldPosition(newPos);
}
```

---

## Transformation Matrices

```cpp
void MyScript::onUpdate(float deltaTime) {
    // Useful for passing to shaders or doing manual matrix math
    const glm::mat4& local = scriptEntity.getLocalMatrix();
    const glm::mat4& world = scriptEntity.getWorldMatrix();
}
```

---

## Hierarchy

Entities support parent-child relationships. Child transforms are relative to their parent.

```cpp
void MyScript::onStart() {
    auto scene = getEntity().getScene();

    auto weapon = scene->getEntity("Weapon");
    auto hand   = scene->getEntity("Hand");

    // Attach weapon to hand - world position is preserved
    weapon.setParent(hand);

    // Detach - weapon becomes a root entity
    weapon.removeFromParent();
    // or: weapon.setParent(Strike::Entity{}); // pass invalid entity to detach

    // Add a child directly
    auto shield = scene->getEntity("Shield");
    hand.addChild(shield);

    // Traverse children
    std::vector<Strike::Entity> children = hand.getChildren();
    for (auto& child : children) {
        // ...
    }

    // Check hierarchy state
    bool isRoot = hand.isRoot();
    Strike::Entity parent = hand.getParent(); // invalid entity if root
}
```

**Setting a parent preserves world position** - the local transform is recalculated automatically so the entity stays where it was in world space.

---

## Physics

```cpp
void MyScript::onUpdate(float deltaTime) {
    // All entities currently overlapping with this one
    auto colliders = scriptEntity.getCollidingEntities();
    for (auto& other : colliders) {
        // react to each colliding entity
    }

    // Check a specific pair
    auto bullet = getEntity().getScene()->getEntity("Bullet");
    if (scriptEntity.isCollidingWith(bullet)) {
        // handle hit
    }
}
```

Collision detection requires a `PhysicsComponent` with `collide` enabled on both entities. See [Scene Format - Physics Component](scene-format.md#physics) for configuration.

---

## Lifecycle

```cpp
void MyScript::onUpdate(float deltaTime) {
    auto projectile = getEntity().getScene()->getEntity("OldProjectile");

    if (projectile) {
        projectile.destroy();
        // projectile handle is now invalid - do not use it after this point
    }
}
```

After calling `destroy()`, the handle and all copies of it are invalid.

---

## Operators & Utilities

```cpp
void MyScript::onStart() {
    auto scene = getEntity().getScene();
    auto a = scene->getEntity("EntityA");
    auto b = scene->getEntity("EntityB");

    if (a == b) { ... }   // same underlying entity
    if (a != b) { ... }

    // Bool check - same as isValid()
    if (a) { ... }

    // Internal EnTT handle (for low-level operations)
    entt::entity handle = a.getHandle();

    // Owning scene
    Strike::Scene* owningScene = a.getScene();
}
```

Entities can be stored in hash-based containers without a custom hasher - `std::hash<Strike::Entity>` is provided automatically:

```cpp
// MyScript.h
std::unordered_map<Strike::Entity, int> mHealthMap;

// MyScript.cpp
void MyScript::onStart() {
    auto scene = getEntity().getScene();
    mHealthMap[scene->getEntity("Player")] = 100;
    mHealthMap[scene->getEntity("Enemy")]  = 50;
}
```

---

## Notes

**Local vs World transforms** - use local transforms when positioning within a hierarchy (e.g. a child bone relative to its parent). Use world transforms when placing something at an absolute position in the scene regardless of parenting.

**Euler angle order** - all Euler angle inputs and outputs are in degrees. Internally the engine uses quaternions; Euler angles are converted on set and decomposed on get.

**Entity copies** - `Entity` is a thin handle (a handle ID and a scene pointer). Copying an entity does not duplicate the underlying data - both copies refer to the same scene object. Use `==` to compare identity.

## Next Step

- [Component API](06-components-api.md)