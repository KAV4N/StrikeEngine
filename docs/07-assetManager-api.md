# AssetManager API

The `AssetManager` is a singleton that handles loading, caching, and lifecycle management for all assets in StrikeEngine. Assets declared in your scene XML are loaded automatically - you only need to interact with the `AssetManager` directly when loading or querying assets at runtime from scripts.

---

## Accessing the AssetManager

```cpp
Strike::AssetManager& am = Strike::AssetManager::get();
```

---

## Supported Asset Types

| Type | XML tag | File formats |
|---|---|---|
| `Model` | `<Model>` | OBJ, FBX, GLTF |
| `Texture` | `<Texture>` | PNG, JPG |
| `CubeMap` | `<CubeMap>` | PNG, JPG |
| `Audio` | `<Audio>` | WAV, MP3, OGG, FLAC |
| `Template` | `<Template>` | `.tmpl` (auto-generated from 3D models) |

---

## Asset States

Every asset progresses through a state machine during loading:

| State | Meaning |
|---|---|
| `Uninitialized` | Created but loading not yet started |
| `Loading` | Background loading in progress |
| `Ready` | Fully loaded and GPU-initialized |
| `Failed` | Loading failed |

Query state on any asset pointer:

```cpp
asset->isReady();
asset->isLoading();
asset->hasFailed();
asset->getLoadingState(); // returns AssetState enum
```

---

## Loading Assets

### Synchronous

Blocks until the asset is fully loaded and ready. Safe to use immediately after the call returns.

```cpp
void MyScript::onStart() {
    auto& am = Strike::AssetManager::get();

    auto model   = am.load<Strike::Model>  ("crate",     "Assets/Models/crate.obj");
    auto texture = am.load<Strike::Texture>("crate_tex", "Assets/Textures/crate.png");
    auto audio   = am.load<Strike::Audio>  ("explosion", "Assets/Sounds/explosion.wav");
}
```

If an asset with the same ID is already loaded and the type matches, the cached instance is returned immediately. If the type doesn't match, an assertion fires in Debug builds and the cast result is returned regardless - treat a type mismatch as a bug in calling code.

> **Template note:** Always pass the source model path (e.g. `.obj`) when loading a `Template` - never the `.tmpl` path directly. The engine generates and caches the `.tmpl` file automatically.

```cpp
// Correct
auto tmpl = am.load<Strike::Template>("player_tmpl", "Assets/Models/player.obj");

// Wrong - assertion fires in Debug
auto tmpl = am.load<Strike::Template>("player_tmpl", "Assets/Models/player.tmpl");
```

### Asynchronous

Returns a placeholder immediately in `Loading` state. The asset transitions to `Ready` after `update()` processes the background thread result. Always check `isReady()` before use.

```cpp
void MyScript::onStart() {
    auto& am = Strike::AssetManager::get();

    mTexture = am.loadAsync<Strike::Texture>("terrain_tex", "Assets/Textures/terrain.png");
}

void MyScript::onUpdate(float deltaTime) {
    if (mTexture && mTexture->isReady()) {
        // safe to use now
    }
}
```

> **Important:** `AssetManager::update()` must be called each frame for async assets to complete GPU initialization. This is handled automatically by the engine loop - you do not need to call it manually.

### Path Resolution

Paths starting with `Assets` are resolved relative to the engine root (your `Assets/` directory). Use this prefix for all asset paths to ensure portability across build configurations.

---

## Retrieving Assets by ID

Use `getAsset<T>()` to retrieve an already-loaded asset by its ID. Returns `nullptr` if the asset is not found. If the asset exists but the type doesn't match, an assertion fires in Debug builds.

```cpp
void MyScript::onStart() {
    auto& am = Strike::AssetManager::get();

    // Returns shared_ptr<Model>, or nullptr if not found
    auto model = am.getAsset<Strike::Model>("player_model");

    if (model && model->isReady()) {
        // use model
    }
}
```

For untyped access (when you only need to check existence or state):

```cpp
void MyScript::onStart() {
    auto& am = Strike::AssetManager::get();

    std::shared_ptr<Strike::Asset> base = am.getAssetBase("player_model");
    if (base) {
        STRIKE_INFO("Type: {}", base->getTypeName());
        STRIKE_INFO("Path: {}", base->getPath().string());
    }
}
```

---

## Querying State

```cpp
void MyScript::onUpdate(float deltaTime) {
    auto& am = Strike::AssetManager::get();

    // Does an asset with this ID exist in the cache?
    if (am.hasAsset("terrain_tex")) { ... }

    // Is a specific asset still loading?
    if (am.isAssetLoading("terrain_tex")) { ... }

    // Are any assets still loading?
    if (am.isLoading()) { ... }

    // Is the manager shutting down?
    if (am.isShuttingDown()) { return; }
}
```

---

## Asset Inventory

```cpp
void MyScript::onStart() {
    auto& am = Strike::AssetManager::get();

    // IDs of all fully ready assets
    std::vector<std::string> readyIds = am.getLoadedAssetIds();

    // IDs of all assets still in Loading state
    std::vector<std::string> loadingIds = am.getLoadingAssetIds();

    // Counts
    size_t ready   = am.getLoadedAssetCount();
    size_t loading = am.getLoadingAssetCount();
}
```

---

## Removing Assets

```cpp
void MyScript::onUpdate(float deltaTime) {
    auto& am = Strike::AssetManager::get();

    // Remove a single asset by ID - releases the shared_ptr from the cache
    am.removeAsset("temp_texture");

    // Remove all assets (safe to call at any time - re-enables loading after clearing)
    am.clear();
}
```

Note that `removeAsset()` only removes the cache entry. If other `shared_ptr` owners (e.g. components) are still holding the asset, it stays alive until those references are released.

---

## Complete Example - Runtime Asset Loading

```cpp
// SpawnOnEvent.h
std::shared_ptr<Strike::Model>   mSpawnModel;
std::shared_ptr<Strike::Texture> mSpawnTexture;
```

```cpp
// SpawnOnEvent.cpp
void SpawnOnEvent::onStart() {
    auto& am = Strike::AssetManager::get();

    mSpawnModel   = am.load<Strike::Model>  ("crate",     "Assets/Models/crate.obj");
    mSpawnTexture = am.load<Strike::Texture>("crate_tex", "Assets/Textures/crate.png");
}

void SpawnOnEvent::onEvent(Strike::Event& event) {
    if (event.getEventType() != Strike::EventType::KeyPressed) return;

    auto& keyEvent = static_cast<Strike::KeyPressedEvent&>(event);
    if (keyEvent.getKeyCode() != STRIKE_KEY_E) return;

    if (!mSpawnModel || !mSpawnModel->isReady()) return;

    auto scene  = getEntity().getScene();
    auto entity = scene->createEntity();

    entity.setWorldPosition(scriptEntity.getWorldPosition() + scriptEntity.getForward() * 3.0f);

    auto& renderer = entity.addComponent<Strike::RendererComponent>();
    renderer.setModel("crate");
    renderer.setTexture("crate_tex");

    auto& physics = entity.addComponent<Strike::PhysicsComponent>();
    physics.setMass(10.0f);

    event.handled = true;
}

void SpawnOnEvent::onDestroy() {
    Strike::AssetManager::get().removeAsset("crate");
    Strike::AssetManager::get().removeAsset("crate_tex");
}

REGISTER_SCRIPT(SpawnOnEvent)
```

---

## Complete Example - Async Loading with Progress Check

```cpp
// LevelLoader.h
std::shared_ptr<Strike::Texture> mTerrainTex;
std::shared_ptr<Strike::Model>   mTerrainModel;
bool mAssetsApplied = false;
```

```cpp
// LevelLoader.cpp
void LevelLoader::onStart() {
    auto& am = Strike::AssetManager::get();

    mTerrainTex   = am.loadAsync<Strike::Texture>("terrain_tex",   "Assets/Textures/terrain.png");
    mTerrainModel = am.loadAsync<Strike::Model>  ("terrain_model", "Assets/Models/terrain.obj");
}

void LevelLoader::onUpdate(float deltaTime) {
    if (mAssetsApplied) return;

    if (mTerrainTex->isReady() && mTerrainModel->isReady()) {
        auto terrain = getEntity().getScene()->getEntity("Terrain");
        if (terrain && terrain.hasComponent<Strike::RendererComponent>()) {
            auto& renderer = terrain.getComponent<Strike::RendererComponent>();
            renderer.setModel("terrain_model");
            renderer.setTexture("terrain_tex");
        }
        mAssetsApplied = true;
    }
}

REGISTER_SCRIPT(LevelLoader)
```

---

## Notes

**Caching** - Assets are keyed by ID. Calling `load<T>()` or `loadAsync<T>()` with an ID that already exists returns the cached instance without re-loading. Requesting a cached ID with a mismatched type triggers an assertion in Debug builds - treat this as a bug, not a recoverable condition.

**Reference counting** - Assets are managed by `std::shared_ptr`. The `AssetManager` holds one reference; every `getAsset<T>()` caller holds another. Calling `removeAsset()` drops the manager's reference but the asset stays alive as long as any other owner holds a copy.

**Type safety** - All template methods enforce `T` must derive from `Asset` via `static_assert` at compile time. Type mismatches at runtime (ID exists but wrong type) trigger an assertion in Debug builds via `STRIKE_ASSERT`.

---

## Next Step

- [Asset API](08-asset-api.md)