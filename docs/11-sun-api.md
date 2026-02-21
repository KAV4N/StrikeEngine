# StrikeEngine Sun API Documentation

## Overview

The `Sun` class in StrikeEngine represents a directional light source that simulates sunlight in your 3D scenes. It provides global illumination, shadow casting capabilities, and can be dynamically controlled through scripts or configured in XML scene files.

---

## Accessing the Sun

The Sun is accessed through the current Scene instance:

```cpp
auto scene = Strike::World::get().getScene();
if (scene) {
    Strike::Sun& sun = scene->getSun();
}
```

---

## Color and Intensity Control

#### `void setColor(const glm::uvec3& color)`
Sets the RGB color of the sunlight (0-255 range). Values are automatically clamped to the valid range.

#### `const glm::uvec3& getColor() const`
Gets the current RGB color of the sunlight.

#### `void setIntensity(float intensity)`
Sets the intensity multiplier (1.0 = normal brightness). Values are clamped to a minimum of 0.0.

#### `float getIntensity() const`
Gets the current intensity value.

---

## Rotation and Direction

### Quaternion-based Rotation

#### `void setRotation(const glm::quat& rotation)`
Sets the sun rotation using a quaternion. The quaternion is normalized internally.

#### `const glm::quat& getRotation() const`
Gets the current rotation as a quaternion.

### Euler Angle Rotation

#### `void setRotationEuler(const glm::vec3& eulerDegrees)`
Sets the rotation using Euler angles in degrees (x, y, z).

#### `glm::vec3 getRotationEuler() const`
Gets the rotation as Euler angles in degrees (x, y, z).

### Incremental Rotation

#### `void rotate(const glm::vec3& eulerDegrees)`
Rotates the sun by the given Euler angles in degrees. Applies x, y, z rotations sequentially around their respective world axes.

#### `void rotateQuaternion(float angleDegrees, const glm::vec3& axis)`
Rotates the sun around a specified axis by a given angle in degrees. Yaw (Y-axis) rotations are applied in world space; all other axes are applied in local space.

### Light Direction

#### `glm::vec3 getDirection() const`
Returns the light direction vector calculated from the rotation quaternion applied to the forward vector (0, 0, -1).

---

## Shadow Control

#### `void setCastShadows(bool cast)`
Enables or disables shadow casting.

#### `bool getCastShadows() const`
Returns true if shadow casting is enabled, false otherwise.

---

## Shadow Mapping

#### `glm::mat4 calculateLightSpaceMatrix(const CameraComponent& camera)`
Calculates the light space matrix (`projection * view`) for shadow mapping. The calculation is based on the camera's frustum, capped at a maximum shadow distance of 40.0 world units (`MAX_SHADOW_DISTANCE`). Uses an orthographic projection fitted tightly around the camera frustum corners in light space, with a small Z pullback (10.0 units) to capture casters behind the frustum.

---

## XML Configuration

The Sun is configured in scene XML files using the `<sun>` tag:

```xml
<sun color="255,245,230" intensity="0.5" rotation="-30,0,0" shadows="false"/>
```

| Attribute   | Description                                      |
|-------------|--------------------------------------------------|
| `color`     | RGB color values (0-255, comma-separated)        |
| `intensity` | Light intensity multiplier                       |
| `rotation`  | Euler angles in degrees (x,y,z)                  |
| `shadows`   | Boolean to enable/disable shadow casting         |

---

## Usage Examples

### Basic Sun Manipulation

```cpp
void updateSun(float deltaTime) {
    auto scene = Strike::World::get().getScene();
    auto& sun = scene->getSun();

    // Rotate sun continuously
    sun.rotate(glm::vec3(10.0f * deltaTime, 0.0f, 0.0f));

    // Change color and intensity
    sun.setColor(glm::uvec3(255, 200, 100));
    sun.setIntensity(1.2f);
}
```

### Day/Night Cycle Script

```cpp
void SunMovement::onStart() {
    auto scene = Strike::World::get().getScene();
    mSun = &scene->getSun();
    mTimeOfDay = 6.0f;
}

void SunMovement::onUpdate(float deltaTime) {
    // Rotate the sun
    float rotationSpeed = 10.0f;
    mSun->rotate(glm::vec3(rotationSpeed * deltaTime, 0.0f, 0.0f));

    // Simulate time of day (0 to 24 hours)
    float daySpeed = 0.1f;
    mTimeOfDay += deltaTime * daySpeed;
    if (mTimeOfDay >= 24.0f)
        mTimeOfDay -= 24.0f;

    // Update sun color based on time of day
    glm::vec3 color = getSunColor(mTimeOfDay);
    mSun->setColor(color);
}
```

### Shadow Configuration

```cpp
void enableShadows() {
    auto scene = Strike::World::get().getScene();
    auto& sun = scene->getSun();
    sun.setCastShadows(true);
}
```

---

## Rendering Integration

The Sun is automatically used by the rendering system for directional lighting:

- Light direction and color are passed to shaders each frame
- Shadow mapping is handled automatically when shadows are enabled
- The light space matrix is calculated for shadow projection based on the camera frustum

---

## Notes

- The Sun uses a quaternion for internal rotation representation but provides Euler angle methods for convenience.
- Color values are clamped to the 0-255 range when set.
- Intensity values are clamped to a minimum of 0.0.
- Only one Sun exists per scene - it is a global directional light source.
- The light direction is the rotation quaternion applied to the forward vector (0, 0, -1), returned without additional normalization.
- Shadow distance is capped internally at `MAX_SHADOW_DISTANCE = 40.0f` world units.

## Next Step

- [Application API](12-application-api.md)