#include "AssetShowcase.h"

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static std::string stateToString(StrikeEngine::AssetState state)
{
    switch (state)
    {
        case StrikeEngine::AssetState::Uninitialized: return "Uninitialized";
        case StrikeEngine::AssetState::Loading:       return "Loading";
        case StrikeEngine::AssetState::Ready:         return "Ready";
        case StrikeEngine::AssetState::Failed:        return "Failed";
        default:                                      return "Unknown";
    }
}

// ------------------------------------------------------------
// Lifecycle
// ------------------------------------------------------------

void AssetShowcase::onStart()
{
    auto* scene = getEntity().getScene();

    mLabel = scene->createEntity();
    auto& text = mLabel.addComponent<StrikeEngine::TextComponent>();
    text.setText("Asset Showcase — Auto-advancing every 2 seconds");
    text.setColor(glm::uvec3(80,220,255));
    text.setPosition(glm::vec2(0.03f,0.03f));
    text.setPivot(glm::vec2(0,0));

    STRIKE_INFO("Asset Showcase ready.");
}

void AssetShowcase::onUpdate(float dt)
{
    if (mWaitingAsync)
    {
        pollAsync();
        return;
    }

    // Use tick to advance every 2 seconds
    if (tick(2.0f))
    {
        advance();
    }
}

// ------------------------------------------------------------
// Async Polling
// ------------------------------------------------------------

void AssetShowcase::pollAsync()
{
    auto asset = StrikeEngine::AssetManager::get().getAssetBase(mAsyncWaitId);
    if (!asset) 
    {
        STRIKE_INFO("Async asset '{}' was removed during loading!", mAsyncWaitId);
        mWaitingAsync = false;
        return;
    }

    auto state = asset->getState();

    if (state == StrikeEngine::AssetState::Ready)
    {
        STRIKE_INFO("Async asset '{}' READY", mAsyncWaitId);
        mWaitingAsync = false;
    }
    else if (state == StrikeEngine::AssetState::Failed)
    {
        STRIKE_INFO("Async asset '{}' FAILED", mAsyncWaitId);
        mWaitingAsync = false;
    }
}

// ------------------------------------------------------------
// Step Machine
// ------------------------------------------------------------

void AssetShowcase::advance()
{
    mStep++;

    switch (mStep)
    {
        case 0:  queryInitialState();      break;
        case 1:  syncLoadModel();          break;
        case 2:  asyncLoadModel();         break;
        case 3:  syncLoadTexture();        break;
        case 4:  asyncLoadTexture();       break;
        case 5:  syncLoadCubeMap();        break;
        case 6:  asyncLoadCubeMap();       break;
        case 7:  syncLoadTemplate();       break;
        case 8:  asyncLoadTemplate();      break;
        case 9:  instantiateTemplate();    break;
        case 10: syncLoadAudio();          break;
        case 11: asyncLoadAudio();         break;
        case 12: testHasAsset();           break;
        case 13: testIsAssetLoading();     break;
        case 14: testIsLoading();          break;
        case 15: testGetAssetBase();       break;
        case 16: queryAssetManager();      break;
        case 17: removeAssetDemo();        break;
        case 18: testRemoveLoadingAsset(); break;
        case 19: testClearAssets();        break;
        case 20: finalQuery();             break;
        default:
            STRIKE_INFO("Asset Showcase complete.");
            updateLabel("Showcase complete!");
            mStep = -1;
            break;
    }
}

// ------------------------------------------------------------
// Label Helper
// ------------------------------------------------------------

void AssetShowcase::updateLabel(const std::string& text)
{
    if (!mLabel.isValid()) return;

    mLabel.getComponent<StrikeEngine::TextComponent>()
        .setText("[Auto-advancing every 2s]\n" + text);
}

// ------------------------------------------------------------
// Steps
// ------------------------------------------------------------

void AssetShowcase::queryInitialState()
{
    auto& am = StrikeEngine::AssetManager::get();

    STRIKE_INFO("=== Initial State ===");
    STRIKE_INFO("Loaded count: {}", am.getLoadedAssetCount());
    STRIKE_INFO("Loading count: {}", am.getLoadingAssetCount());
    STRIKE_INFO("Is shutting down: {}", am.isShuttingDown());

    updateLabel("Queried initial state");
}

void AssetShowcase::syncLoadModel()
{
    auto model = StrikeEngine::AssetManager::get()
        .load<StrikeEngine::Model>(ID_MODEL_SYNC, "Assets/Objects/box/box.obj");

    if (model)
        STRIKE_INFO("Model sync loaded. State: {}",
            stateToString(model->getState()));

    updateLabel("Sync model loaded");
}

void AssetShowcase::asyncLoadModel()
{
    auto model = StrikeEngine::AssetManager::get()
        .loadAsync<StrikeEngine::Model>(ID_MODEL_ASYNC, "Assets/Objects/box/portalCube/CCP2.fbx");

    if (model)
    {
        STRIKE_INFO("Async model loading...");
        mWaitingAsync = true;
        mAsyncWaitId = ID_MODEL_ASYNC;
    }

    updateLabel("Async model started");
}

void AssetShowcase::syncLoadTexture()
{
    auto tex = StrikeEngine::AssetManager::get()
        .load<StrikeEngine::Texture>(ID_TEX_SYNC, "Assets/Objects/tempoTrack/Root_baseColor.png");

    if (tex)
        STRIKE_INFO("Texture sync loaded.");

    updateLabel("Sync texture loaded");
}

void AssetShowcase::asyncLoadTexture()
{
    auto tex = StrikeEngine::AssetManager::get()
        .loadAsync<StrikeEngine::Texture>(ID_TEX_ASYNC, "Assets/Objects/tempoTrack/road.png");

    if (tex)
    {
        STRIKE_INFO("Async texture loading...");
        mWaitingAsync = true;
        mAsyncWaitId = ID_TEX_ASYNC;
    }

    updateLabel("Async texture started");
}

void AssetShowcase::syncLoadCubeMap()
{
    auto cube = StrikeEngine::AssetManager::get()
        .load<StrikeEngine::CubeMap>(
            ID_CUBEMAP_SYNC,
            "Assets/Skyboxes/mountainSkybox"
        );

    if (cube)
        STRIKE_INFO("CubeMap sync loaded.");

    updateLabel("Sync CubeMap loaded");
}

void AssetShowcase::asyncLoadCubeMap()
{
    auto cube = StrikeEngine::AssetManager::get()
        .loadAsync<StrikeEngine::CubeMap>(
            ID_CUBEMAP_ASYNC,
             "Assets/Skyboxes/plainSkybox"
        );

    if (cube)
    {
        STRIKE_INFO("Async CubeMap loading...");
        mWaitingAsync = true;
        mAsyncWaitId = ID_CUBEMAP_ASYNC;
    }

    updateLabel("Async CubeMap started");
}


void AssetShowcase::syncLoadTemplate()
{
    auto tmpl = StrikeEngine::AssetManager::get()
        .load<StrikeEngine::Template>(
            ID_TEMPLATE_SYNC,
            "Assets/Objects/box/box.obj"
        );

    if (tmpl)
        STRIKE_INFO("Template sync loaded.");

    updateLabel("Template loaded");
}

void AssetShowcase::asyncLoadTemplate()
{
    auto tmpl = StrikeEngine::AssetManager::get()
        .loadAsync<StrikeEngine::Template>(
            ID_TEMPLATE_ASYNC,
            "Assets/Objects/box/portalCube/CCP2.fbx"
        );

    if (tmpl)
    {
        STRIKE_INFO("Async Template loading...");
        mWaitingAsync = true;
        mAsyncWaitId = ID_TEMPLATE_ASYNC;
    }

    updateLabel("Async Template started");
}

void AssetShowcase::instantiateTemplate()
{
    auto tmpl = StrikeEngine::AssetManager::get()
        .getAsset<StrikeEngine::Template>(ID_TEMPLATE_SYNC);

    if (!tmpl)
    {
        STRIKE_INFO("Template not found.");
        return;
    }

    if (tmpl->getState() != StrikeEngine::AssetState::Ready)
    {
        STRIKE_INFO("Template not ready.");
        return;
    }

    auto* scene = getEntity().getScene();
    StrikeEngine::Entity entity = scene->createEntity();

    if (tmpl->instantiate(entity))
    {
        STRIKE_INFO("Template instantiated.");
        updateLabel("Template instantiated");
    }
    else
    {
        STRIKE_INFO("Template failed.");
        updateLabel("Template failed");
    }
}

void AssetShowcase::syncLoadAudio()
{
    auto audio = StrikeEngine::AssetManager::get()
        .load<StrikeEngine::Audio>(
            ID_AUDIO_SYNC,
            "Assets/Sounds/shot.mp3"
        );

    if (audio)
        STRIKE_INFO("Audio sync loaded.");

    updateLabel("Audio loaded");
}

void AssetShowcase::asyncLoadAudio()
{
    auto audio = StrikeEngine::AssetManager::get()
        .loadAsync<StrikeEngine::Audio>(
            ID_AUDIO_ASYNC,
            "Assets/Sounds/ambient.mp3"
        );

    if (audio)
    {
        STRIKE_INFO("Async audio loading...");
        mWaitingAsync = true;
        mAsyncWaitId = ID_AUDIO_ASYNC;
    }

    updateLabel("Async audio started");
}

void AssetShowcase::testHasAsset()
{
    auto& am = StrikeEngine::AssetManager::get();

    STRIKE_INFO("=== Testing hasAsset() ===");
    STRIKE_INFO("Has '{}': {}", ID_MODEL_SYNC, am.hasAsset(ID_MODEL_SYNC));
    STRIKE_INFO("Has '{}': {}", ID_TEX_SYNC, am.hasAsset(ID_TEX_SYNC));
    STRIKE_INFO("Has 'non_existent': {}", am.hasAsset("non_existent"));

    updateLabel("Tested hasAsset()");
}

void AssetShowcase::testIsAssetLoading()
{
    auto& am = StrikeEngine::AssetManager::get();

    STRIKE_INFO("=== Testing isAssetLoading() ===");
    
    // Start an async load for testing
    auto testAsset = am.loadAsync<StrikeEngine::Texture>(
        ID_TEST_ASYNC, 
        "Assets/Objects/tempoTrack/Root_baseColor.png"
    );
    
    STRIKE_INFO("Is '{}' loading: {}", ID_TEST_ASYNC, am.isAssetLoading(ID_TEST_ASYNC));
    STRIKE_INFO("Is '{}' loading: {}", ID_MODEL_SYNC, am.isAssetLoading(ID_MODEL_SYNC));
    STRIKE_INFO("Is 'non_existent' loading: {}", am.isAssetLoading("non_existent"));

    updateLabel("Tested isAssetLoading()");
}

void AssetShowcase::testIsLoading()
{
    auto& am = StrikeEngine::AssetManager::get();

    STRIKE_INFO("=== Testing isLoading() ===");
    STRIKE_INFO("AssetManager is loading any assets: {}", am.isLoading());

    updateLabel("Tested isLoading()");
}

void AssetShowcase::testGetAssetBase()
{
    auto& am = StrikeEngine::AssetManager::get();

    STRIKE_INFO("=== Testing getAssetBase() ===");
    
    auto baseAsset = am.getAssetBase(ID_MODEL_SYNC);
    if (baseAsset)
    {
        STRIKE_INFO("Retrieved base asset '{}', Type: {}, State: {}", 
            baseAsset->getId(), 
            baseAsset->getTypeName(),
            stateToString(baseAsset->getState()));
    }
    else
    {
        STRIKE_INFO("Base asset '{}' not found", ID_MODEL_SYNC);
    }

    auto nonExistent = am.getAssetBase("non_existent");
    STRIKE_INFO("Non-existent asset returned: {}", nonExistent ? "valid" : "nullptr");

    updateLabel("Tested getAssetBase()");
}

void AssetShowcase::queryAssetManager()
{
    auto& am = StrikeEngine::AssetManager::get();

    STRIKE_INFO("=== Querying AssetManager ===");
    STRIKE_INFO("Loaded count: {}", am.getLoadedAssetCount());
    STRIKE_INFO("Loading count: {}", am.getLoadingAssetCount());
    
    STRIKE_INFO("Loaded assets:");
    for (auto& id : am.getLoadedAssetIds())
        STRIKE_INFO(" - {}", id);
    
    STRIKE_INFO("Loading assets:");
    for (auto& id : am.getLoadingAssetIds())
        STRIKE_INFO(" - {}", id);

    updateLabel("Queried AssetManager");
}

void AssetShowcase::removeAssetDemo()
{
    auto& am = StrikeEngine::AssetManager::get();
    
    STRIKE_INFO("=== Testing removeAsset() ===");
    STRIKE_INFO("Before removal - Has '{}': {}", ID_MODEL_SYNC, am.hasAsset(ID_MODEL_SYNC));
    
    am.removeAsset(ID_MODEL_SYNC);
    
    STRIKE_INFO("After removal - Has '{}': {}", ID_MODEL_SYNC, am.hasAsset(ID_MODEL_SYNC));
    STRIKE_INFO("Removed asset '{}'", ID_MODEL_SYNC);
    
    updateLabel("Asset removed");
}

void AssetShowcase::testRemoveLoadingAsset()
{
    auto& am = StrikeEngine::AssetManager::get();
    
    STRIKE_INFO("=== Testing removal of loading asset ===");
    
    // Start an async load
    auto asyncAsset = am.loadAsync<StrikeEngine::Model>(
        "test_remove_loading", 
        "Assets/Objects/box/portalCube/CCP2.fbx"
    );
    
    STRIKE_INFO("Started async load, is loading: {}", am.isAssetLoading("test_remove_loading"));
    
    // Immediately remove it while loading
    am.removeAsset("test_remove_loading");
    
    STRIKE_INFO("Removed during loading, has asset: {}", am.hasAsset("test_remove_loading"));
    
    // Try to get it
    auto retrieved = am.getAssetBase("test_remove_loading");
    STRIKE_INFO("Retrieved after removal: {}", retrieved ? "valid (shouldn't happen)" : "nullptr (correct)");

    updateLabel("Tested remove during load");
}

void AssetShowcase::testClearAssets()
{
    auto& am = StrikeEngine::AssetManager::get();
    
    STRIKE_INFO("=== Testing clear() ===");
    STRIKE_INFO("Before clear - Loaded: {}, Loading: {}", 
        am.getLoadedAssetCount(), 
        am.getLoadingAssetCount());
    
    am.clear();
    
    STRIKE_INFO("After clear - Loaded: {}, Loading: {}", 
        am.getLoadedAssetCount(), 
        am.getLoadingAssetCount());
    
    STRIKE_INFO("All assets cleared!");

    updateLabel("All assets cleared");
}

void AssetShowcase::finalQuery()
{
    auto& am = StrikeEngine::AssetManager::get();

    STRIKE_INFO("=== Final State Query ===");
    STRIKE_INFO("Loaded count: {}", am.getLoadedAssetCount());
    STRIKE_INFO("Loading count: {}", am.getLoadingAssetCount());
    STRIKE_INFO("Is loading: {}", am.isLoading());
    STRIKE_INFO("Is shutting down: {}", am.isShuttingDown());

    updateLabel("Final query complete");
}

REGISTER_SCRIPT(AssetShowcase);