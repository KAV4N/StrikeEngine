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
| `tag` | string | `"DefaultScene"` |Optional identifier for this scene |

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

| Attribute | Description |
|---|---|
| `near` / `nearPlane` | Near clipping plane |
| `far` / `farPlane` | Far clipping plane |
| `fov` | Field of view in degrees |
| `order` / `renderOrder` | Render order priority |
| `x` / `viewportX` | Viewport X position (0.0–1.0) |
| `y` / `viewportY` | Viewport Y position (0.0–1.0) |
| `width` / `viewportWidth` | Viewport width (0.0–1.0) |
| `height` / `viewportHeight` | Viewport height (0.0–1.0) |

---

### `<renderer>`

| Attribute | Description |
|---|---|
| `model` | Model asset ID |
| `texture` | Texture asset ID (optional) |
| `color` | RGB color tint `"R,G,B"` (default: `"255,255,255"`) |
| `mesh` | Specific mesh index from the model (optional) |

---

### `<physics>`

| Attribute | Default | Description |
|---|---|---|
| `anchored` | `false` | Static body (does not move) |
| `collide` | `true` | Enable collision detection |
| `mass` | `1.0` | Mass in kilograms |
| `friction` | `0.5` | Friction coefficient |
| `restitution` | `0.0` | Bounciness |
| `lDamping` | `0.0` | Linear damping |
| `aDamping` | `0.05` | Angular damping |

Collision box size is defined with a child element:

```xml
<physics anchored="false" mass="70" friction="0.6">
  <size x="1" y="2" z="1"/>
</physics>
```

> **Note:** The `size="X,Y,Z"` attribute format is not supported. Use the `<size x="" y="" z=""/>` child element.

---

### `<light>`

| Attribute | Default | Description |
|---|---|---|
| `color` | `"255,255,255"` | RGB color (0–255) |
| `intensity` | - | Light intensity |
| `radius` | - | Light radius |
| `fallOff` | - | Falloff distance (optional) |

---

### `<text>`

| Attribute | Default | Description |
|---|---|---|
| `text` | - | Text content to display |
| `color` | `"255,255,255"` | RGB color |
| `pivot` | - | Pivot point `"X,Y"` (0.0–1.0) |
| `x` | - | Horizontal screen position (0.0–1.0) |
| `y` | - | Vertical screen position (0.0–1.0) |

---

### `<audiosource>`

| Attribute | Default | Description |
|---|---|---|
| `audio` | - | Audio asset ID |
| `volume` | `1.0` | Volume level |
| `loop` | `false` | Loop the audio |
| `spatial` | `false` | Enable 3D spatial audio |
| `minDistance` | `1.0` | Minimum attenuation distance |
| `maxDistance` | `50.0` | Maximum attenuation distance |

---

### `<audiolistener>`

Marks the entity as the audio listener for 3D spatial audio. No attributes beyond `active`.

---

### `<logic>`

Attaches scripts to the entity. Add one `<script>` child per script class.

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

    <entity template="player_template" tag="Ground" position="0,0,0">
      <components>
        <renderer model="ground_model" texture="ground_tex"/>
        <physics anchored="true">
          <size x="50" y="1" z="50"/>
        </physics>
      </components>
    </entity>

    <entity template="player_template"/>

    <entity tag="Player" position="0,2,0">
      <components>
        <physics mass="70" lDamping="0.2" aDamping="0.5">
          <size x="1" y="2" z="1"/>
        </physics>
        <audiolistener/>
        <logic>
          <script type="PlayerController"/>
        </logic>
      </components>
      <entity tag="Camera" position="0,1,0">
        <components>
          <camera fov="75" near="0.1" far="500"/>
        </components>
      </entity>
    </entity>

    <entity tag="Ambient" position="0,0,0">
      <components>
        <audiosource audio="ambient" loop="true" volume="0.4"/>
      </components>
    </entity>

  </entities>

</scene>
```
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

## Next Step

- [Template Format](03-template-format.md) - Learn how the templates work in XML