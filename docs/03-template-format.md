# Template Format

Template files are XML files with a `.tmpl` extension that define reusable entity hierarchies with components. They are automatically generated when loading 3D model files and can be instantiated multiple times within a scene.

> **Note:** Templates cannot be nested inside other templates. This is by design to prevent circular references.

---

## File Structure

A template file has three parts:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<template>

  <assets>
    <!-- model and texture declarations -->
  </assets>

  <entities>
    <!-- entity hierarchy with components -->
  </entities>

</template>
```

The `<template>` root element takes no attributes.

---

### `<assets>` Section

Declares the models and textures referenced by the entities inside this template. Uses the same asset syntax as scene files.

```xml
<assets>
  <model id="my_object" src="../Models/my_object.obj"/>
  <texture id="my_object_tex" src="../Textures/my_object_diffuse.png"/>
</assets>
```

See [Scene Format - Assets](02-scene-format.md#assets) for the full list of supported asset types and attributes.

---

### `<entities>` Section

Defines the entity hierarchy. This mirrors the scene format, with full support for nested `<entity>` children and `<components>`.

```xml
<entities>
  <entity tag="Root" position="0,0,0" rotation="0,0,0" scale="1,1,1">
    <components>
      <renderer model="my_object" mesh="0" color="255,255,255"/>
    </components>
    <entity tag="Child" position="0,1,0">
      <components>
        <renderer model="my_object" mesh="1" color="200,200,200"/>
      </components>
    </entity>
  </entity>
</entities>
```

---

## Supported Components in Templates

All scene components are valid inside template entity hierarchies. The most commonly used in templates are:

### `<renderer>`

Assigns a model mesh and optional texture or color tint to the entity.

| Attribute | Default | Description |
|---|---|---|
| `model` | - | Model asset ID (declared in the template's `<assets>`) |
| `texture` | - | Texture asset ID (optional) |
| `color` | `"255,255,255"` | RGB color tint `"R,G,B"` |
| `mesh` | - | Specific mesh index from the model. Used to target a single submesh in multi-mesh models. |

```xml
<renderer model="crate_model" texture="crate_tex" mesh="0" color="255,255,255"/>
```

---

### `<physics>`

Adds a rigid body to the entity. When a `<renderer>` is also present, collision size is automatically derived from the model/mesh bounds on the **first** body creation.

| Attribute | Default | Description |
|---|---|---|
| `anchored` | `false` | Static body (infinite mass, ignores forces) |
| `collide` | `true` | Enable collision detection |
| `mass` | `1.0` | Mass in kilograms (ignored when anchored) |
| `friction` | `0.5` | Friction coefficient |
| `restitution` | `0.0` | Bounciness (`0.0` = no bounce, `1.0` = perfect bounce) |
| `lDamping` | `0.0` | Linear damping |
| `aDamping` | `0.05` | Angular damping |
| `lockRot` | `"0,0,0"` | Per-axis rotation lock `"X,Y,Z"` where `1` = locked, `0` = free |
| `size` | `"1,1,1"` | Collision box dimensions `"X,Y,Z"`. Overridden by renderer bounds on first creation only. |

Collision box size can also be defined with a `<size>` child element:

```xml
<physics anchored="false" mass="20" friction="0.6" restitution="0.1" size="1,1,1"/>
```

---

### `<light>`

Adds a point light source to the entity.

| Attribute | Default | Description |
|---|---|---|
| `color` | `"255,255,255"` | RGB color (0–255 per channel) |
| `intensity` | `1.0` | Light intensity multiplier |
| `radius` | `10.0` | Light influence radius in world units |
| `fallOff` | - | Falloff distance (optional) |

```xml
<light color="255,200,100" intensity="4" radius="15.0"/>
```

---

### `<audiosource>`

Attaches an audio source to the entity with optional spatial audio.

| Attribute | Default | Description |
|---|---|---|
| `audio` | - | Audio asset ID |
| `volume` | `1.0` | Volume level |
| `loop` | `false` | Loop the audio |
| `spatial` | `false` | Enable 3D spatial audio |
| `minDistance` | `1.0` | Full-volume distance |
| `maxDistance` | `50.0` | Silent distance |

```xml
<audiosource audio="engine_sfx" loop="true" volume="0.6" spatial="true" minDistance="2" maxDistance="20"/>
```

---

### `<logic>`

Attaches one or more scripts to the entity. Only one instance of each script type can exist per entity.

```xml
<logic>
  <script type="RotateScript"/>
  <script type="DamageZone"/>
</logic>
```

The `type` attribute must match the registered script class name exactly.

---

## Automatic Generation

Template files are generated automatically by the `ModelParser` when a `<template>` asset is declared in your scene file pointing to a 3D model. You generally don't need to write them by hand.

**What the parser does:**

- Walks the model's node hierarchy and creates a matching `<entity>` tree
- Decomposes each node's transform matrix into `position`, `rotation`, and `scale`
- Extracts material colors from the 3D file and sets them on `<renderer>` components
- Assigns a `mesh` index to each geometry node so each renderer targets the correct submesh

**To trigger generation**, declare a `<template>` asset in your scene's `<assets>` section:

```xml
<assets>
  <template id="vehicle" src="../Models/vehicle.obj"/>
</assets>
```

The engine will parse `vehicle.obj`, generate `vehicle.tmpl` alongside it, and cache it for reuse.

---

## Instantiating a Template in a Scene

Once declared as an asset, use the `template` attribute on any `<entity>` to stamp out an instance:

```xml
<assets>
  <template id="vehicle" src="../Models/vehicle.tmpl"/>
</assets>

<entities>
  <entity tag="Car1" position="0,0,0" template="vehicle"/>
  <entity tag="Car2" position="10,0,0" template="vehicle"/>
</entities>
```

Each instance is independent - overriding `position`, `rotation`, and `scale` on the entity affects only that instance.

---

## Full Example

```xml
<?xml version="1.0" encoding="UTF-8"?>
<template>

  <assets>
    <model id="crate_model" src="@/Assets/Models/crate.obj"/>
    <texture id="crate_tex" src="../Textures/crate_diffuse.png"/>
  </assets>

  <entities>
    <entity tag="Crate" position="0,0,0" rotation="0,0,0" scale="1,1,1">
      <components>
        <renderer model="crate_model" texture="crate_tex" mesh="0" color="255,255,255"/>
        <physics anchored="false" mass="20" friction="0.6" restitution="0.1" size="1,1,1"/>
      </components>
    </entity>
  </entities>

</template>
```

---

## Limitations

- **No nested templates** - a `.tmpl` file cannot reference another template asset. Use scene-level entity nesting instead.
- **Asset paths are relative to the template file** - use the `../` prefix to reference paths from the project root, or `@/` for root-relative paths.
- **Auto-generated files may be overwritten** - if you manually edit a `.tmpl` file, avoid re-triggering generation from the same source model or your edits will be lost.
- **Physics size auto-derivation applies on first creation only** - when a `<renderer>` is present, the physics body size is derived from the model bounds on initial creation. Manual `size` values take effect on subsequent body recreations.
- **One script type per entity** - duplicate `<script type="..."/>` entries on the same entity will be rejected at runtime with a `STRIKE_CORE_ERROR`.

---

## Next Step

- [Script API](04-script-api.md)