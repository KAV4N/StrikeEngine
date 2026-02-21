# StrikeEngine Scene API Documentation

Based on the StrikeEngine codebase, here are all the public methods and properties available in the `Scene` class:

## Entity Management Methods

### Creating Entities

#### `Entity createEntity()`
Creates a new root entity.

#### `Entity createEntity(const Entity& parent)`
Creates a new entity with a specified parent.

#### `void destroy(entt::entity entity)`
Destroys an entity and its children.

### Parent-Child Hierarchy

#### `void setParent(const Entity& child, const Entity& parent)`
Sets the parent of a child entity.

#### `void addChild(const Entity& parent, const Entity& child)`
Adds a child to a parent entity.

#### `bool isAncestor(const Entity& ancestor, const Entity& descendant) const`
Checks ancestor relationship.

#### `bool isDescendant(const Entity& descendant, const Entity& ancestor) const`
Checks descendant relationship.

---

## Entity Query Methods

#### `Entity getEntityFromHandle(entt::entity handle)`
Gets an entity by its EnTT handle.

#### `Entity getEntity(std::string tag)`
Gets an entity by its tag.

---

## Entity Component System (ECS) Methods

### Component Operations

#### `template<typename Component, typename... Args> decltype(auto) addComponent(entt::entity entity, Args&&... args)`
Adds a component to an entity.

#### `template<typename Component> void removeComponent(entt::entity entity)`
Removes a component from an entity.

#### `template<typename Component> Component& getComponent(entt::entity entity)`
Gets a component from an entity.

#### `template<typename Component> const Component& getComponent(entt::entity entity) const`
Gets a const component.

#### `template<typename Component> bool hasComponent(entt::entity entity) const`
Checks if an entity has a component.

### Direct Registry Access

#### `entt::registry& getRegistry()`
Gets the EnTT registry for direct ECS operations.

#### `const entt::registry& getRegistry() const`
Gets the const EnTT registry.

---

## Skybox Methods

#### `void setSkybox(const std::string& cubeMapId)`
Sets the skybox cubemap.

#### `bool hasSkybox() const`
Checks if a skybox is set.

#### `const std::string& getSkyboxId() const`
Gets the skybox asset ID.

#### `std::shared_ptr<CubeMap> getSkybox() const`
Gets the skybox cubemap asset.

---

## Sun (Lighting) Methods

#### `Sun& getSun()`
Gets the Sun object representing the scene's sunlight.

#### `const Sun& getSun() const`
Gets the const Sun object.

---

## Scene Identification

#### `const std::string& getTag()`
Gets the optional tag of the scene.

## Next Step

- [Sun API](11-sun-api.md)
