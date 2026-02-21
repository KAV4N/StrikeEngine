# StrikeEngine World API Documentation

The `World` class in StrikeEngine is a singleton that serves as the central orchestrator for the engine. Here are all the public methods and properties available:

## Singleton Access

#### `static World& get()`
Returns the singleton instance of the World class.

---

## Scene Management

#### `void loadScene(const std::filesystem::path& path)`
Loads a scene from the specified file path. The actual loading is deferred to the end of the frame, and the new scene becomes active in the next frame.

#### `Scene* getScene() const`
Returns a pointer to the currently active scene (may be nullptr if no scene is loaded).

---

## Raycasting

#### `RayHit rayCast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const`
Performs a raycast in the physics world and returns the first hit. If no collision is detected, the returned `RayHit` will have an invalid entity.

#### `std::vector<RayHit> rayCastAll(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const`
Performs a raycast and returns all hits along the ray path. Returns an empty vector if no collisions are detected.

---

## Physics Settings (Gravity)

#### `void setGravity(const glm::vec3& gravity)`
Sets the global gravity vector for the physics world, affecting all physics-enabled entities in the scene.

#### `glm::vec3 getGravity() const`
Returns the current global gravity vector of the physics world.

---

## Fog Configuration

#### `void setFog(float start, float end, float density, const glm::uvec3& color)`
Sets the fog settings for the world. Set density to 0 to disable fog.

#### `const Fog& getFog() const`
Returns a const reference to the current fog settings.

---

## Supporting Structures

### Fog Structure

The `Fog` struct contains the following properties:

```cpp
struct Fog {
    float start = 0.0f;                          // Distance at which fog starts
    float end = 100.0f;                          // Distance at which fog ends
    float density = 0.0f;                        // Fog density (0 = disabled)
    glm::uvec3 color = glm::uvec3(255, 255, 255); // Fog color in RGB (default: white)
};
```

### RayHit Structure

The `RayHit` struct returned by raycasting methods contains:

```cpp
struct RayHit {
    Entity entity;                      // The entity that was hit
    glm::vec3 normal = glm::vec3(0.0f); // Surface normal at the hit point
    float distance = 0.0f;              // Distance from ray origin to hit point

    bool hasHit() const { return entity.isValid(); } // Check if the raycast hit anything
};
```

---

## Notes

The `World` class follows the singleton pattern and is designed to be accessed via `World::get()`. It manages the entire engine's state including the current scene and all major systems (`RenderSystem`, `PhysicsSystem`, `AudioSystem`, `ScriptSystem`). The class is non-copyable and non-movable by design, ensuring there is only one instance throughout the application lifetime.


## Next Step

- [Scene API](10-scene-api.md)