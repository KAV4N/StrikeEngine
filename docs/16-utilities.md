# StrikeEngine Utilities API Documentation

---

## Log / Logging API

For engine-internal use only:

```cpp
STRIKE_CORE_TRACE(...)   // Trace-level debug message
STRIKE_CORE_INFO(...)    // Informational message
STRIKE_CORE_WARN(...)    // Warning message
STRIKE_CORE_ERROR(...)   // Error message
```

### Client Logger Macros

For use in your game or application scripts:

```cpp
STRIKE_TRACE(...)        // Trace-level debug message
STRIKE_INFO(...)         // Informational message
STRIKE_WARN(...)         // Warning message
STRIKE_ERROR(...)        // Error message
```

### Usage Examples

```cpp
// Informational output
STRIKE_INFO("Player spawned at ({}, {}, {})", x, y, z);

// Warning
STRIKE_WARN("Health below 20%: {}", currentHealth);

// Error
STRIKE_ERROR("Failed to load asset '{}'", assetId);

// Format strings work like printf/fmt
STRIKE_INFO("Score: {0}, Lives: {1}", score, lives);
```
---

## ComponentRegistry API

### Overview

The `ComponentRegistry` is a static factory registry that enables runtime component creation from XML scene files. Components register themselves automatically using the `REGISTER_COMPONENT` macro.

You generally do not need to use this class directly in scripts - it is used internally by the scene loading system.

> **Note**
> 
> This feature is currently **experimental**.
>
> You can create your own custom components by extending the `Component` class located at:
>
> ```
> StrikeEngine/src/Scene/Components/Component.h
> ```
>
> To ensure your component works correctly when used in XML files, it must implement all required methods defined in the base `Component` class.
>
> Failure to implement the required methods may result in unexpected behavior.

### Static Methods

#### `static void registerComponentFactory(const std::string& typeName, ComponentFactory factory)`
Registers a component factory under the given type name. Asserts if the type name is already registered - duplicate registrations are not allowed.

#### `static bool hasComponentFactory(const std::string& typeName)`
Returns true if a factory is registered for the given type name, false otherwise.

#### `static void addComponentToEntity(Entity& entity, const std::string& typeName, const pugi::xml_node& node)`
Creates and adds a component of the specified type to the given entity, using the XML node for deserialization. Silently does nothing if the type is not registered.

#### `static std::vector<std::string> getRegisteredComponents()`
Returns a vector of all registered component type names. Useful for debugging or editor tooling.

### REGISTER_COMPONENT Macro

Every built-in and custom component that should be loadable from XML must be registered:

```cpp
REGISTER_COMPONENT(MyComponent)
```

Place this at the top of the component's `.cpp` file. The macro uses static initialization to register the factory before `main()` is called. The component must implement `getStaticTypeName()` and `deserialize(const pugi::xml_node&)`.

```cpp
// Example - LogicComponent.cpp
REGISTER_COMPONENT(LogicComponent)
```

---

## ScriptRegistry API

### Overview

The `ScriptRegistry` enables dynamic script registration and instantiation by class name, allowing scripts to be attached to entities from XML scene files at runtime. Scripts register themselves using the `REGISTER_SCRIPT` macro.

See the [Scripting API](SCRIPTING_API.md) for full script authoring documentation.

### Static Methods

#### `static void registerScriptFactory(const std::string& className, ScriptFactory factory)`
Registers a script factory under the given class name. Asserts if the class name is already registered.

#### `static bool hasScriptFactory(const std::string& className)`
Returns true if a factory is registered for the given class name, false otherwise.

#### `static std::unique_ptr<Script> createScript(const std::string& className)`
Creates and returns a new instance of the script with the given class name. Returns `nullptr` if the class name is not found in the registry.

#### `static std::vector<std::string> getRegisteredScripts()`
Returns a vector of all registered script class names. Useful for debugging, UI script selectors, or tooling.

### REGISTER_SCRIPT Macro

Every script that should be loadable from XML must be registered:

```cpp
REGISTER_SCRIPT(MyScript)
```

Place this at the bottom of the script's `.cpp` file. The macro uses static initialization to register the factory before `main()` is called. The string key is the class name as written - it must match exactly what is used in the `type="..."` XML attribute.

```cpp
// MyScript.cpp
void MyScript::onUpdate(float deltaTime) { ... }

REGISTER_SCRIPT(MyScript)
```

### Notes

**Ownership** - Scripts are managed with `std::unique_ptr` and owned by `LogicComponent`. The engine handles their full lifecycle.

