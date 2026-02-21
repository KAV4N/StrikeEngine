# StrikeEngine Component API Documentation

## Table of Contents

- [CameraComponent](#cameracomponent)
- [RendererComponent](#renderercomponent)
- [PhysicsComponent](#physicscomponent)
- [LightComponent](#lightcomponent)
- [TextComponent](#textcomponent)
- [AudioSourceComponent](#audiosourcecomponent)
- [AudioListenerComponent](#audiolistenercomponent)
- [LogicComponent](#logiccomponent)

---

## CameraComponent

The `CameraComponent` defines the perspective projection, view frustum, and viewport for rendering scenes. It maintains view and projection matrices and supports frustum culling.

### Constructors

#### `CameraComponent()`
Default constructor that initializes camera with default FOV (45°), near plane (0.1), and far plane (1000).

#### `CameraComponent(float fov, float nearPlane, float farPlane)`
Construct camera with custom projection parameters.

### Static Methods

#### `static const std::string& getStaticTypeName()`
Returns the static type name "camera" for this component.

### Projection Settings

#### `void setPerspective(float fov, float nearPlane, float farPlane)`
Set perspective projection parameters.

#### `void setFOV(float fov)`
Set the field of view in degrees.

#### `float getFOV() const`
Get the field of view in degrees.

#### `void setNearPlane(float nearPlane)`
Set the near clipping plane distance.

#### `float getNearPlane() const`
Get the near clipping plane distance.

#### `void setFarPlane(float farPlane)`
Set the far clipping plane distance.

#### `float getFarPlane() const`
Get the far clipping plane distance.

### Viewport Management

#### `void setViewportRect(float x, float y, float width, float height)`
Set the viewport rectangle using normalized coordinates (0-1).

#### `const Rect& getViewportRect() const`
Get the viewport rectangle.

### Matrix Access

#### `glm::mat4 getProjectionMatrix() const`
Get the 4x4 projection matrix.

#### `glm::mat4 getViewMatrix() const`
Get the 4x4 view matrix.

#### `glm::mat4 getViewProjectionMatrix() const`
Get the combined view-projection matrix.

### Render Order

#### `int getRenderOrder() const`
Get the camera's render order (lower values render first).

#### `void setRenderOrder(int order)`
Set the camera's render order for multi-camera setups.

### Camera Control

#### `void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f))`
Orient the camera to look at a target position.

#### `void update(const glm::mat4& worldMatrix, uint32_t width, uint32_t height)`
Update camera matrices and frustum. Should be called each frame to update camera state.

### Frustum Culling

#### `const Frustum& getFrustum() const`
Get the view frustum containing 6 planes for culling tests.

### Usage Example

XML scene configuration:

```xml
<camera
    nearPlane="0.1"
    farPlane="100.0"
    fov="60.0"
    viewportX="0.0"
    viewportY="0.0"
    viewportWidth="1.0"
    viewportHeight="1.0"
    renderOrder="2"
/>
```

Script usage for camera movement:

```cpp
void CameraMovement::onStart() {
    mCameraEntity = getEntity();
}

void CameraMovement::onUpdate(float deltaTime) {
    if (glm::length(move) > 0.0f) {
        mCameraEntity.move(move);
    }

    if (glm::length(rotation) > 0.0f)
        mCameraEntity.rotate(rotation);
}
```

---

## RendererComponent

The `RendererComponent` manages the visual representation of an entity, including model/mesh assignment, texture mapping, and color tinting.

### Constructor

#### `RendererComponent()`
Default constructor.

### Static Methods

#### `static const std::string& getStaticTypeName()`
Returns the static type name "renderer" for this component.

### Model Management

#### `void setModel(const std::string& modelId)`
Set the model to render using its asset ID.

#### `void removeModel()`
Remove the currently assigned model.

#### `bool hasModel() const`
Check if a model is assigned.

#### `const std::string& getModelId() const`
Get the ID of the currently assigned model.

#### `std::shared_ptr<Model> getModel() const`
Get the model resource object (or nullptr if not set).

### Texture Management

#### `void setTexture(const std::string& textureId)`
Set the texture to apply to the model using its asset ID.

#### `void removeTexture()`
Remove the currently assigned texture.

#### `bool hasTexture() const`
Check if a texture is assigned.

#### `const std::string& getTextureId() const`
Get the ID of the currently assigned texture.

#### `std::shared_ptr<Texture> getTexture() const`
Get the texture resource object (or nullptr if not set).

### Color Tinting

#### `void setColor(const glm::uvec3& color)`
Set the color tint for rendering with RGB values (0-255 per channel).

#### `glm::uvec3 getColor() const`
Get the color tint as RGB values (0-255).

### Mesh Selection

#### `void setMesh(const std::string& modelId, uint32_t meshIndex)`
Set a specific mesh from a model to render (for rendering only a single mesh from a multi-mesh model).

#### `std::shared_ptr<Mesh> getMesh() const`
Get the currently selected mesh (or nullptr if no specific mesh is set).

#### `bool hasMesh() const`
Check if a specific mesh is selected.

#### `std::optional<uint32_t> getMeshIdx() const`
Get the index of the selected mesh (or empty if not set).

### Usage Example

Programmatic usage:

```cpp
StrikeEngine::Entity entity = scene->createEntity();
auto& renderer = entity.addComponent<StrikeEngine::RendererComponent>();
renderer.setMesh("box", 0);
```

XML scene configuration:

```xml
<renderer model="gizmo" material="gizmo_DefaultMaterial"/>
```

---

## PhysicsComponent

The `PhysicsComponent` provides rigid body physics using the Bullet physics engine. It supports properties like mass, velocity, friction, and collision detection.

### Constructor

#### `PhysicsComponent()`
Default constructor.

### Static Methods

#### `static const std::string& getStaticTypeName()`
Returns the static type name "physics" for this component.

### Anchored/Static Body Control

#### `void setAnchored(bool anchored)`
Set whether the physics body is anchored/static (true) or dynamic (false). Anchored bodies have infinite mass and don't respond to forces.

#### `bool isAnchored() const`
Check if the physics body is anchored.

### Collision Control

#### `void setCanCollide(bool canCollide)`
Set whether this body can collide with others.

#### `bool canCollide() const`
Check if collision detection is enabled.

### Mass

#### `void setMass(float mass)`
Set the mass in kilograms (ignored for anchored bodies).

#### `float getMass() const`
Get the mass in kilograms.

### Collision Shape

#### `void setSize(const glm::vec3& size)`
Set the collision box dimensions in world units.

#### `const glm::vec3& getSize() const`
Get the collision box dimensions.

#### `void setCenter(const glm::vec3& center)`
Set the collision box center offset from entity position.

#### `const glm::vec3& getCenter() const`
Get the collision box center offset.

### Velocity Control

#### `void setVelocity(const glm::vec3& velocity)`
Set the linear velocity in world space.

#### `glm::vec3 getVelocity() const`
Get the current linear velocity in world space.

#### `void setAngularVelocity(const glm::vec3& angularVel)`
Set the angular velocity (axis-angle representation).

#### `glm::vec3 getAngularVelocity() const`
Get the current angular velocity.

### Force Application

#### `void applyImpulse(const glm::vec3& impulse)`
Apply an impulse force in world space at the center of mass.

### Physical Properties

#### `void setFriction(float friction)`
Set the friction coefficient (typically 0.0 to 1.0).

#### `float getFriction() const`
Get the friction coefficient.

#### `void setBounciness(float restitution)`
Set the bounciness/restitution coefficient (0.0 = no bounce, 1.0 = perfect bounce).

#### `float getBounciness() const`
Get the bounciness coefficient.

### Damping

#### `void setLinearDamping(float damping)`
Set the linear damping factor to gradually reduce linear velocity over time (0.0 = no damping).

#### `float getLinearDamping() const`
Get the linear damping factor.

#### `void setAngularDamping(float damping)`
Set the angular damping factor to gradually reduce angular velocity over time (0.0 = no damping).

#### `float getAngularDamping() const`
Get the angular damping factor.

### Usage Example

Programmatic usage:

```cpp
auto& physics = entity.addComponent<StrikeEngine::PhysicsComponent>();
physics.setCanCollide(true);
physics.setAnchored(false);
physics.setMass(1.0f);
float throwSpeed = 500.0f;
physics.setVelocity(forward * throwSpeed);
physics.setAngularVelocity(glm::vec3(100, 0, 0));
```

XML scene configuration:

```xml
<physics anchored="true" collide="true" mass="1" size="10,10,10"/>
```

### RayHit Structure

Used for raycast results:

```cpp
struct RayHit {
    Entity entity;                      // The entity that was hit
    glm::vec3 normal = glm::vec3(0.0f); // Surface normal at hit point
    float distance = 0.0f;              // Distance from ray origin to hit point

    bool hasHit() const { return entity.isValid(); }
};
```

---

## LightComponent

The `LightComponent` represents a point light source with color, intensity, and radius properties for illuminating the scene.

### Constructors

#### `LightComponent()`
Default constructor creating a white light with default intensity (1.0) and radius (10.0).

#### `LightComponent(const glm::uvec3& color, float intensity, float radius)`
Construct light with specific properties.

### Static Methods

#### `static const std::string& getStaticTypeName()`
Returns the static type name "light" for this component.

### Color

#### `void setColor(const glm::uvec3& color)`
Set the light color with RGB values (0-255 per channel). Values are automatically clamped to the valid range.

#### `const glm::uvec3& getColor() const`
Get the light color as RGB values (0-255).

### Intensity

#### `void setIntensity(float intensity)`
Set the light intensity multiplier (>= 0.0). Negative values are clamped to 0.0.

#### `float getIntensity() const`
Get the light intensity multiplier.

### Radius

#### `void setRadius(float radius)`
Set the light influence radius in world units.

#### `float getRadius() const`
Get the light influence radius in world units.

### Usage Example

Programmatic usage (spawning lights dynamically):

```cpp
void CameraMovement::spawnLight() {
    auto scene = mCameraEntity.getScene();
    auto newEnt = scene->createEntity();

    newEnt.setPosition(mCameraEntity.getPosition());
    auto& pointLight = newEnt.addComponent<StrikeEngine::LightComponent>();

    float r = static_cast<float>(rand() % 256);
    float g = static_cast<float>(rand() % 256);
    float b = static_cast<float>(rand() % 256);

    pointLight.setColor(glm::vec3(r, g, b));
    pointLight.setRadius(5);
    pointLight.setIntensity(5);
}
```

XML scene configuration:

```xml
<light color="255,245,200" intensity="8" radius="100.0" fallOff="25"/>
```

---

## TextComponent

The `TextComponent` manages 2D text rendering with properties for content, color, position, and pivot point. Positions and pivots use normalized coordinates (0-1).

### Constructor

#### `TextComponent()`
Default constructor.

### Static Methods

#### `static const std::string& getStaticTypeName()`
Returns the static type name "text" for this component.

### Text Content

#### `void setText(const std::string& text)`
Set the text content to display.

#### `const std::string& getText() const`
Get the text content.

### Color

#### `void setColor(const glm::uvec3& color)`
Set the text color with RGB values (0-255 per channel). Values are automatically clamped to the valid range.

#### `const glm::uvec3& getColor() const`
Get the text color as RGB values (0-255).

### Position

#### `void setPosition(const glm::vec2& position)`
Set the screen position using normalized coordinates (0-1), where (0,0) = top-left and (1,1) = bottom-right.

#### `const glm::vec2& getPosition() const`
Get the screen position as normalized coordinates (0-1).

### Pivot

#### `void setPivot(const glm::vec2& pivot)`
Set the pivot point for text alignment using normalized coordinates (0-1), where (0,0) = top-left corner, (0.5,0.5) = center, (1,1) = bottom-right.

#### `const glm::vec2& getPivot() const`
Get the pivot point as normalized coordinates (0-1).

### Usage Example

Programmatic usage:

```cpp
entity.getComponent<StrikeEngine::TextComponent>().setText(
    "Time of Day: " + std::to_string(static_cast<int>(mTimeOfDay)) + ":00"
);
```

XML scene configuration:

```xml
<text text="Strike Engine Demo" color="255,255,255" pivot="0.5,0.5" x="0.5" y="0.5"/>
```

---

## AudioSourceComponent

The `AudioSourceComponent` manages audio playback with 3D spatial audio support, including control over volume, looping, and distance attenuation parameters.

### Constructor

#### `AudioSourceComponent()`
Default constructor.

### Static Methods

#### `static const std::string& getStaticTypeName()`
Returns the static type name "audiosource" for this component.

### Audio Asset Management

#### `void setAudio(const std::string& audioId)`
Set the audio clip to be played using its asset ID.

#### `void removeAudio()`
Remove the currently assigned audio clip.

#### `const std::string& getAudioId() const`
Get the ID of the currently assigned audio clip.

#### `std::shared_ptr<Audio> getAudio() const`
Get the audio resource object (or nullptr if not set).

#### `bool hasAudio() const`
Check if an audio clip is assigned.

### Playback Control

#### `void play()`
Start playing the audio clip. Playback will begin on the next audio system update.

#### `void stop()`
Stop playing the audio clip. Playback will stop on the next audio system update.

### Volume

#### `void setVolume(float volume)`
Set the playback volume (0.0 to 1.0+, where 1.0 is full volume).

#### `float getVolume() const`
Get the current playback volume.

### Looping

#### `void setLoop(bool loop)`
Enable or disable audio looping (true to loop, false to play once).

#### `bool isLoop() const`
Check if audio looping is enabled.

### Spatial Audio

#### `void setSpatial(bool spatial)`
Enable or disable 3D spatial audio (true for 3D, false for 2D). Spatial audio is affected by distance and position relative to the AudioListener.

#### `bool isSpatial() const`
Check if spatial audio is enabled.

### Distance Attenuation

#### `void setMinDistance(float distance)`
Set the minimum distance for distance attenuation in world units. Audio plays at full volume within this distance.

#### `float getMinDistance() const`
Get the minimum distance for distance attenuation.

#### `void setMaxDistance(float distance)`
Set the maximum distance for distance attenuation in world units. Audio volume reaches zero at this distance.

#### `float getMaxDistance() const`
Get the maximum distance for distance attenuation.

### Usage Example

Programmatic usage:

```cpp
StrikeEngine::Entity ent = scene->getEntity("shotSound");
if (ent.isValid()) {
    auto& audio = ent.getComponent<StrikeEngine::AudioSourceComponent>();
    if (audioPlayed == false) {
        audio.play();
        audioPlayed = true;
    }
    else if (tick(audio.getAudio()->getDuration()))
        audioPlayed = false;
}
```

XML scene configuration:

```xml
<audiosource audio="shot" autoplay="true" volume="0.1" spatial="true"/>
```

---

## AudioListenerComponent

The `AudioListenerComponent` represents the position and orientation from which the player/camera hears 3D audio. Only one active audio listener should exist in the scene at a time.

### Constructor

#### `AudioListenerComponent()`
Default constructor.

### Static Methods

#### `static const std::string& getStaticTypeName()`
Returns the static type name "audiolistener" for this component.

### Usage Example

XML scene configuration (typically attached to camera entity):

```xml
<audiolistener/>
```

---

## LogicComponent

The `LogicComponent` allows multiple Script instances to be attached to an entity, providing custom behavior and game logic. Scripts are stored and managed internally as unique pointers.

> **Note:** Only one instance of each script type can exist on a given entity at a time. Attempting to add a duplicate type - either programmatically via `addScript<T>()` or through XML - will be rejected with a `STRIKE_CORE_ERROR` log, and the existing instance will be returned instead. To replace a script, call `removeScript<T>()` first.

### Constructor

#### `LogicComponent()`
Default constructor.

### Static Methods

#### `static const std::string& getStaticTypeName()`
Returns the static type name "logic" for this component.

### Script Management

#### `template<typename T> T* addScript()`
Add a script of the specified type to this component. Returns a pointer to the newly created script instance. The component takes ownership of the script. If a script of the same type is already attached, logs a `STRIKE_CORE_ERROR` and returns the existing instance instead.

#### `template<typename T> void removeScript()`
Remove the script of the specified type.

#### `template<typename T> T* getScript()`
Get the script of the specified type. Returns nullptr if not found.

#### `template<typename T> const T* getScript() const`
Get the script of the specified type (const overload). Returns nullptr if not found.

#### `template<typename T> bool hasScript() const`
Check if a script of the specified type is attached.

#### `const std::vector<std::unique_ptr<Script>>& getScripts() const`
Get all scripts attached to this component (const).

#### `std::vector<std::unique_ptr<Script>>& getScripts()`
Get all scripts attached to this component.

#### `void clearScripts()`
Remove all scripts from this component.

### Usage Example

Programmatic usage:

```cpp
auto& logic = entity.addComponent<StrikeEngine::LogicComponent>();

// Add - logs STRIKE_CORE_ERROR and returns existing instance if type is already attached
logic.addScript<DeathTimer>();

// Get (returns nullptr if not found)
DeathTimer* timer = logic.getScript<DeathTimer>();

// Check
if (logic.hasScript<DeathTimer>()) { ... }

// Remove
logic.removeScript<DeathTimer>();
```

**To replace a script of a given type:**

```cpp
logic.removeScript<DeathTimer>(); // remove existing instance first
logic.addScript<DeathTimer>();    // then add a fresh one
```

XML scene configuration:

```xml
<logic>
  <script type="PlayerController"/>
  <script type="HealthSystem"/>
</logic>
```

## Next Step

- [Asset Manager API](07-assetManager-api.md)