# Scene Format

Scenes are defined in XML and loaded at runtime. The file must begin with an XML declaration and a root `<scene>` tag.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scene tag="MyScene">
  <!-- assets, skybox, sun, fog, entities -->
</scene>
```

**`<scene>` attributes:**

| Attribute | Type | Default | Description |
|---|---|---|---|
| `tag` | string | `"DefaultScene"` | Optional identifier for this scene |

---

## Top-Level Elements

The `<scene>` element supports five child elements in any order: `<assets>`, `<skybox>`, `<sun>`, `<fog>`, `<entities>`.

---

## `<assets>`

Declares all resources used in the scene. Supports the `@/` prefix for root-relative paths.

| Element | Description |
|---|---|
| `<model>` | 3D model (OBJ, FBX, GLTF, etc.) |
| `<template>` | Reusable entity template automatically generated from model |
| `<texture>` | Image texture (PNG, JPG, etc.) |
| `<cubeMap>` | Skybox cubemap |
| `<audio>` | Audio file (MP3, WAV, etc.) |

**Common attributes for all asset types:**

| Attribute | Required | Description |
|---|---|---|
| `id` | Yes | Unique identifier referenced by components |
| `src` | Yes | File path to the asset |
| `async` | No | Load asynchronously (boolean) |

```xml
<assets>
  <model id="player_model" src="@/Models/player.obj"/>
  <template id="player_template" src="@/Models/player.obj"/>
  <texture id="player_tex" src="@/Textures/player.png"/>
  <audio id="jump_sfx" src="@/Sounds/jump.wav"/>
  <cubeMap id="sky" src="@/Textures/skybox"/>
</assets>
```

---

## `<skybox>`

| Attribute | Required | Description |
|---|---|---|
| `cubeMap` | Yes | Asset ID of the cubemap to use |

```xml
<skybox cubeMap="sky"/>
```

---

## `<sun>`

| Attribute | Default | Description |
|---|---|---|
| `color` | `"255,255,255"` | RGB color (0–255 per channel) |
| `intensity` | `1.0` | Light intensity |
| `rotation` | `"0,0,0"` | Euler angles X,Y,Z |
| `shadows` | `false` | Enable shadow casting |

```xml
<sun color="255,240,200" intensity="1.2" rotation="45,30,0" shadows="true"/>
```

---

## `<fog>`

| Attribute | Default | Description |
|---|---|---|
| `start` | `10.0` | Distance where fog begins |
| `end` | `100.0` | Distance where fog is fully opaque |
| `density` | `0.015` | Fog density |
| `color` | `"128,153,179"` | RGB color |

```xml
<fog start="20" end="80" density="0.02" color="150,160,170"/>
```

---

## `<entities>`

Entities are the core building block of a scene. They can be nested to create parent-child hierarchies.

**Entity attributes:**

| Attribute | Default | Description |
|---|---|---|
| `position` | `"0,0,0"` | World position X,Y,Z |
| `rotation` | `"0,0,0"` | Euler angles X,Y,Z |
| `scale` | `"1,1,1"` | Scale X,Y,Z |
| `tag` | - | Optional tag for identifying the entity |
| `active` | `true` | Whether the entity is active |
| `template` | - | Template asset ID to instantiate |

Each entity can contain a `<components>` element and nested `<entity>` children.

```xml
<entities>
  <entity tag="Player" position="0,1,0">
    <components>
      <!-- component definitions -->
    </components>
    <entity tag="PlayerCamera" position="0,2,0">
      <components>
        <camera fov="75" near="0.1" far="1000"/>
      </components>
    </entity>
  </entity>
</entities>
```

---

## Components

Components are defined inside a `<components>` element. All components support the `active` boolean attribute.

---

### `<camera>`

Defines perspective projection, view frustum, and viewport for rendering. Supports frustum culling.

| Attribute | Alias | Default | Description |
|---|---|---|---|
| `fov` | - | `45` | Field of view in degrees |
| `near` | `nearPlane` | `0.1` | Near clipping plane distance |
| `far` | `farPlane` | `1000` | Far clipping plane distance |
| `order` | `renderOrder` | - | Render order priority (lower values render first) |
| `x` | `viewportX` | `0.0` | Viewport X position (0.0–1.0) |
| `y` | `viewportY` | `0.0` | Viewport Y position (0.0–1.0) |
| `width` | `viewportWidth` | `1.0` | Viewport width (0.0–1.0) |
| `height` | `viewportHeight` | `1.0` | Viewport height (0.0–1.0) |

```xml
<camera fov="75" near="0.1" far="500"
        viewportX="0.0" viewportY="0.0"
        viewportWidth="1.0" viewportHeight="1.0"
        renderOrder="0"/>
```

---

### `<renderer>`

Manages the visual representation of an entity, including model/mesh assignment, texture mapping, and color tinting.

| Attribute | Default | Description |
|---|---|---|
| `model` | - | Model asset ID |
| `texture` | - | Texture asset ID (optional) |
| `color` | `"255,255,255"` | RGB color tint `"R,G,B"` |
| `mesh` | - | Specific mesh index from the model (optional) |

```xml
<renderer model="player_model" texture="player_tex" color="255,255,255" mesh="0"/>
```

---

### `<physics>`

Provides rigid body physics using the Bullet physics engine. Supports mass, velocity, friction, collision detection, and per-axis rotation locking.

| Attribute | Default | Description |
|---|---|---|
| `anchored` | `false` | Static body (does not move; infinite mass, ignores forces) |
| `collide` | `true` | Enable collision detection |
| `mass` | `1.0` | Mass in kilograms (ignored when anchored) |
| `friction` | `0.5` | Friction coefficient |
| `restitution` | `0.0` | Bounciness coefficient (`0.0` = no bounce, `1.0` = perfect bounce) |
| `lDamping` | `0.0` | Linear damping (reduces linear velocity over time) |
| `aDamping` | `0.05` | Angular damping (reduces angular velocity over time) |
| `lockRot` | `"0,0,0"` | Per-axis rotation lock as `"X,Y,Z"` where `1` = locked, `0` = free |
| `size` | `"1,1,1"` | Collision box dimensions as `"X,Y,Z"`. Overridden by renderer bounds on first creation only. |

Collision box size can also be set with a `<size>` child element:

```xml
<physics anchored="false" mass="70" friction="0.6" lDamping="0.2" aDamping="0.5">
  <size x="1" y="2" z="1"/>
</physics>
```

> **Note:** The `size="X,Y,Z"` attribute format and the `<size x="" y="" z=""/>` child element are both supported for defining the collision box. When a `<renderer>` is also present on the entity, the collision size is automatically derived from the model/mesh bounds on the **first** body creation and will override any `size` value set in XML or in `onStart()`. Use the `size` attribute or child element on subsequent recreations.

> **Rotation locking** only applies to dynamic (non-anchored) bodies.

```xml
<!-- Static ground -->
<physics anchored="true" collide="true"/>

<!-- Character controller: all rotation locked -->
<physics anchored="false" collide="true" mass="80" lockRot="1,1,1">
  <size x="1" y="2" z="1"/>
</physics>

<!-- Dynamic body, only Y rotation free -->
<physics anchored="false" collide="true" mass="2"
         friction="0.4" restitution="0.1" lDamping="0.0" aDamping="0.05"
         lockRot="1,0,1" size="2.0,2.0,2.0"/>
```

---

### `<light>`

Represents a point light source with color, intensity, and radius.

| Attribute | Default | Description |
|---|---|---|
| `color` | `"255,255,255"` | RGB color (0–255 per channel) |
| `intensity` | `1.0` | Light intensity multiplier (`>= 0.0`) |
| `radius` | `10.0` | Light influence radius in world units |
| `fallOff` | - | Falloff distance (optional) |

```xml
<light color="255,245,200" intensity="8" radius="100.0" fallOff="25"/>
```

---

### `<text>`

Manages 2D text rendering with screen-space positioning using normalized coordinates (0–1).

| Attribute | Default | Description |
|---|---|---|
| `text` | - | Text content to display |
| `color` | `"255,255,255"` | RGB color (0–255 per channel) |
| `x` | - | Horizontal screen position (0.0–1.0), where `0` = left, `1` = right |
| `y` | - | Vertical screen position (0.0–1.0), where `0` = top, `1` = bottom |
| `pivot` | - | Pivot/alignment point `"X,Y"` (0.0–1.0). `"0.5,0.5"` = center |

```xml
<text text="Strike Engine Demo" color="255,255,255" pivot="0.5,0.5" x="0.5" y="0.5"/>
```

---

### `<audiosource>`

Manages audio playback with optional 3D spatial audio and distance attenuation.

| Attribute | Default | Description |
|---|---|---|
| `audio` | - | Audio asset ID |
| `volume` | `1.0` | Volume level (`0.0` to `1.0+`) |
| `loop` | `false` | Loop the audio |
| `spatial` | `false` | Enable 3D spatial audio (requires an `<audiolistener>` in the scene) |
| `minDistance` | `1.0` | Distance at which audio plays at full volume |
| `maxDistance` | `50.0` | Distance at which audio volume reaches zero |

```xml
<audiosource audio="ambient" loop="true" volume="0.4" spatial="true" minDistance="2" maxDistance="30"/>
```

---

### `<audiolistener>`

Marks the entity as the audio listener for 3D spatial audio. Only one active audio listener should exist in the scene at a time. Typically attached to the player or camera entity.

No attributes beyond `active`.

```xml
<audiolistener/>
```

---

### `<logic>`

Attaches one or more scripts to the entity. Each `<script>` child binds a registered script class.

> **Note:** Only one instance of each script type can exist on a given entity at a time. Duplicate types will be rejected with a `STRIKE_CORE_ERROR` and the existing instance will be returned instead. To replace a script, remove it first via `removeScript<T>()`.

```xml
<logic>
  <script type="PlayerController"/>
  <script type="HealthSystem"/>
</logic>
```

The `type` attribute must match the registered script class name exactly.

---

## Full Example

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scene tag="MainScene">

  <assets>
    <model id="ground_model" src="../Models/ground.obj"/>
    <model id="player_model" src="../Models/player.obj"/>
    <texture id="ground_tex" src="@/Textures/ground.png"/>
    <audio id="ambient" src="@/Sounds/ambient.mp3"/>
    <cubeMap id="sky" src="@/Textures/sky"/>
    <template id="player_template" src="@/Models/player.obj"/>
  </assets>

  <skybox cubeMap="sky"/>
  <sun color="255,240,210" intensity="1.0" rotation="50,30,0" shadows="true"/>
  <fog start="30" end="150" color="180,190,200"/>

  <entities>

    <!-- Static ground plane -->
    <entity tag="Ground" position="0,0,0">
      <components>
        <renderer model="ground_model" texture="ground_tex"/>
        <physics anchored="true">
          <size x="50" y="1" z="50"/>
        </physics>
      </components>
    </entity>

    <!-- Template instance -->
    <entity template="player_template"/>

    <!-- Player with physics, audio listener, and scripted logic -->
    <entity tag="Player" position="0,2,0">
      <components>
        <physics anchored="false" mass="70" lDamping="0.2" aDamping="0.5" lockRot="1,1,1">
          <size x="1" y="2" z="1"/>
        </physics>
        <audiolistener/>
        <logic>
          <script type="PlayerController"/>
        </logic>
      </components>

      <!-- Child camera entity -->
      <entity tag="Camera" position="0,1,0">
        <components>
          <camera fov="75" near="0.1" far="500"/>
        </components>
      </entity>
    </entity>

    <!-- Ambient audio -->
    <entity tag="Ambient" position="0,0,0">
      <components>
        <audiosource audio="ambient" loop="true" volume="0.4"/>
      </components>
    </entity>

    <!-- Point light -->
    <entity tag="SunLight" position="0,10,0">
      <components>
        <light color="255,245,200" intensity="8" radius="100.0" fallOff="25"/>
      </components>
    </entity>

    <!-- HUD text -->
    <entity tag="HUD">
      <components>
        <text text="Strike Engine" color="255,255,255" pivot="0.5,0.0" x="0.5" y="0.05"/>
      </components>
    </entity>

  </entities>

</scene>
```

---

### Load Your Scene

```cpp
#include <StrikeEngine.h>

int main(int argc, char** argv) {
    Strike::Application app;

    auto& world = Strike::World::get();
    world.loadScene("Assets/Scenes/ExampleScene.xml");

    app.run();
    return 0;
}
```

---

## Next Step

- [Template Format](03-template-format.md) - Learn how templates work in XML