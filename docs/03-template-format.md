# Template Format

Template files are XML files with a `.tmpl` extension that define reusable entity hierarchies with components. They are automatically generated when loading 3D model files and can be instantiated multiple times within a scene.

> **Note:** Templates cannot be nested inside other templates. This is by design to prevent circular references.

---

## File Structure

A template file has three parts:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<template name="my_object" source="../Models/my_object.obj">

  <assets>
    <!-- model and texture declarations -->
  </assets>

  <entities>
    <!-- entity hierarchy with components -->
  </entities>

</template>
```

### `<template>` Root Attributes

| Attribute | Description |
|---|---|
| `name` | Identifier for this template (optional) |
| `source` | Path to the originating 3D model file (optional) |

---

### `<assets>` Section

Declares the models and textures referenced by the entities inside this template. Uses the same asset syntax as scene files.

```xml
<assets>
  <model id="my_object" src="../Models/my_object.obj"/>
  <texture id="my_object_tex" src="../Textures/my_object_diffuse.png"/>
</assets>
```

See [Scene Format - Assets](scene-format.md#assets) for the full list of supported asset types and attributes.

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
<template name="crate" source="../Models/crate.obj">

  <assets>
    <model id="crate_model" src="@/Assets/Models/crate.obj"/>
    <texture id="crate_tex" src="../Textures/crate_diffuse.png"/>
  </assets>

  <entities>
    <entity tag="Crate" position="0,0,0" rotation="0,0,0" scale="1,1,1">
      <components>
        <renderer model="crate_model" texture="crate_tex" mesh="0" color="255,255,255"/>
        <physics anchored="false" mass="20" friction="0.6" restitution="0.1">
          <size x="1" y="1" z="1"/>
        </physics>
      </components>
    </entity>
  </entities>

</template>
```

---

## Limitations

- **No nested templates** - a `.tmpl` file cannot reference another template asset. Use scene-level entity nesting instead.
- **Asset paths are relative to the template file** - use the `../` prefix to reference paths from the project root.
- **Auto-generated files may be overwritten** - if you manually edit a `.tmpl` file, avoid re-triggering generation from the same source model or your edits will be lost.

## Next Step

- [Script API](04-script-api.md)