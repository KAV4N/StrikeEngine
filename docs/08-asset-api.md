# StrikeEngine Asset API Documentation

All asset classes in StrikeEngine inherit from a common `Asset` base class. Below is the base class interface followed by each specific asset type.

---

## Base Asset Class

All asset types (`Model`, `Texture`, `CubeMap`, `Audio`, `Template`) inherit these public methods:

### Constructor

#### `Asset(const std::string& id, const std::filesystem::path& path)`
Constructs an asset with a unique identifier and file path. Does not load the asset; use `AssetManager` for loading.

### Getters

#### `const std::string& getId() const`
Returns the unique asset identifier.

#### `AssetState getState() const`
Returns the current loading state (`Uninitialized`, `Loading`, `Ready`, or `Failed`).

#### `const std::filesystem::path& getPath() const`
Returns the file path to the asset.

#### `virtual const std::string& getTypeName() const`
Returns the static type name of the asset (pure virtual, implemented by subclasses).

### State Check Methods

#### `bool isReady() const`
Returns true if the asset is fully loaded and ready to use.

#### `bool isLoading() const`
Returns true if the asset is currently being loaded.

#### `bool hasFailed() const`
Returns true if the asset failed to load.

### Utility Methods

---

## Model Asset

The `Model` class represents 3D model assets with meshes.

### Constructor

#### `Model(const std::string& id, const std::filesystem::path& path)`
Constructs a new Model object. Does not load the model; use `AssetManager` for loading.

### Type Methods

#### `static const std::string& getStaticTypeName()`
Returns `"model"` as the static type name.

#### `const std::string& getTypeName() const override`
Returns the type name (overrides Asset base class).

### Mesh Access Methods

#### `const std::vector<std::shared_ptr<Mesh>>& getMeshes() const`
Returns a const reference to all meshes in the model. Warning: reference becomes invalid if the model is destroyed.

#### `const std::shared_ptr<Mesh> getMesh(uint32_t index) const`
Returns a specific mesh by index. Returns nullptr if index is out of range.

#### `uint32_t getMeshCount() const`
Returns the number of meshes in the model.

### Bounds Methods

#### `const Bounds& getBounds() const`
Returns the overall bounding box of the model. Warning: reference becomes invalid if the model is destroyed.

### Supporting Classes

**Mesh Class** - public methods:

```cpp
const std::vector<Vertex>& getVertices() const; // Returns mesh vertices
const std::vector<uint32_t>& getIndices() const; // Returns mesh indices
const Bounds& getBounds() const;                 // Returns the mesh bounding box
GLuint getVAO() const;                           // Returns the OpenGL Vertex Array Object handle
GLuint getInstanceVBO() const;                   // Returns the OpenGL Instance Buffer Object handle for instanced rendering
```

**Bounds Struct** - public methods:

```cpp
Bounds();                                                      // Default constructor
Bounds(const glm::vec3& p);                                    // Construct from a single point
Bounds(const glm::vec3& min, const glm::vec3& max);            // Construct from min and max points
Bounds(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c); // Construct from three points
void unionBounds(const Bounds& bounds);                        // Expand to include another bounding box
float area() const;                                            // Calculate the surface area
glm::vec3 getMidPoint() const;                                 // Calculate the midpoint
```

---

## Texture Asset

The `Texture` class represents 2D texture assets.

### Constructor

#### `Texture(const std::string& id, const std::filesystem::path& path)`
Constructs a new Texture object.

### Type Methods

#### `static const std::string& getStaticTypeName()`
Returns `"texture"` as the static type name.

#### `const std::string& getTypeName() const override`
Returns the type name (overrides Asset base class).

### Getters

#### `GLuint getTextureID() const`
Returns the OpenGL texture ID.

#### `int getWidth() const`
Returns the texture width in pixels.

#### `int getHeight() const`
Returns the texture height in pixels.

#### `int getChannels() const`
Returns the number of color channels (e.g., 3 for RGB, 4 for RGBA).

---

## CubeMap Asset

The `CubeMap` class represents cube map textures, typically used for skyboxes and environment mapping.

### Constructor

#### `CubeMap(const std::string& id, const std::filesystem::path& path)`
Constructs a new CubeMap object.

### Type Methods

#### `static const std::string& getStaticTypeName()`
Returns `"cubeMap"` as the static type name.

#### `const std::string& getTypeName() const override`
Returns the type name (overrides Asset base class).

### Getters

#### `GLuint getTextureID() const`
Returns the OpenGL texture ID.

#### `int getWidth() const`
Returns the width of each cube face in pixels.

#### `int getHeight() const`
Returns the height of each cube face in pixels.

#### `int getChannels() const`
Returns the number of color channels.

---

## Audio Asset

The `Audio` class represents audio file assets.

### Constructor

#### `Audio(const std::string& id, const std::filesystem::path& path)`
Constructs a new Audio object. Does not load the audio; use `AssetManager` for loading.

### Type Methods

#### `static const std::string& getStaticTypeName()`
Returns `"audio"` as the static type name.

#### `const std::string& getTypeName() const override`
Returns the type name (overrides Asset base class).

### Getters

#### `float getDuration() const`
Returns the duration of the audio in seconds.

---

## Template Asset

The `Template` class represents prefab/template assets that can be instantiated as entities in the scene.

### Constructor

#### `Template(const std::string& id, const std::filesystem::path& path)`
Constructs a new Template object. Does not load the template; use `AssetManager` for loading.

### Type Methods

#### `static const std::string& getStaticTypeName()`
Returns `"template"` as the static type name.

#### `const std::string& getTypeName() const override`
Returns the type name (overrides Asset base class).

### Utility Methods

#### `bool instantiate(Entity parentEntity)`
Instantiates the template into the given parent entity. Returns true if successful, false otherwise.

#### `bool areAssetsReady() const`
Checks if all referenced assets are ready. Loops through all referenced assets and checks their loading state.

---

## Notes

### Asset Loading
All assets should be loaded through the `AssetManager` singleton, not by directly calling constructors. The constructors don't perform the actual loading operation.

### Loading States
Assets go through different loading states defined by the `AssetState` enum:

```cpp
enum class AssetState {
    Uninitialized = 0,
    Loading,
    Ready,
    Failed
};
```

### Two-Phase Loading
Some assets (`Model`, `Texture`, `CubeMap`) require a two-phase loading process with a `postLoad()` method that performs GPU resource initialization.

### Reference Validity
Several getter methods return const references with the caveat that these references become invalid if the asset is destroyed. Do not store these references long-term.

## Next Step

- [World API](09-world-api.md)