#include "AssetManagerTest.h"


void AssetManagerTest::check(const std::string& label, bool condition)
{
    if (condition)
    {
        mPassCount++;
    }
    else
    {
        STRIKE_ERROR("[AssetManagerTest] FAIL  {}", label);
        mFailCount++;
    }
}

void AssetManagerTest::printSummary()
{
    STRIKE_INFO("========================================");
    STRIKE_INFO("[AssetManagerTest] Results: {}/{} passed, {} failed",
                mPassCount, mPassCount + mFailCount, mFailCount);
    if (mFailCount == 0)
        STRIKE_INFO("[AssetManagerTest] ALL TESTS PASSED");
    else
        STRIKE_ERROR("[AssetManagerTest] SOME TESTS FAILED");
    STRIKE_INFO("========================================");
}

// =====================================================================
// Lifecycle
// =====================================================================

void AssetManagerTest::onStart()
{
    STRIKE_INFO("========================================");
    STRIKE_INFO("[AssetManagerTest] Starting tests");
    STRIKE_INFO("========================================");

    runSyncLoads();
    runSyncImmediateTests();
    runSyncMissingTests();
    runAsyncKickoff();
}

void AssetManagerTest::onUpdate(float deltaTime)
{
    auto& am = Strike::AssetManager::get();

    if (am.isLoading())
    {
        if (tick(0.5f))
            STRIKE_INFO("[AssetManagerTest] {} still loading...", am.getLoadingAssetCount());
        return;
    }

    if (!mSyncPostLoadValidated)
    {
        runSyncPostLoadValidation();
        mSyncPostLoadValidated = true;
        return;
    }

    if (!mAsyncValidated)
    {
        runAsyncValidation();
        mAsyncValidated = true;
        return;
    }

    if (!mCleanupValidated)
    {
        runCleanupTests();
        mCleanupValidated = true;
        runCancelKickoff();
        return;
    }

    if (!mCancelValidated)
    {
        runCancelValidation();
        mCancelValidated = true;
        printSummary();
    }
}

void AssetManagerTest::onDestroy()
{
    STRIKE_INFO("[AssetManagerTest] onDestroy");

    auto& am = Strike::AssetManager::get();
    am.removeAsset(kIdCancelModel);
    am.removeAsset(kIdCancelTexture);
    am.removeAsset(kIdCancelAudio);
    am.removeAsset(kIdCancelCubeMap);
    am.removeAsset(kIdCancelTemplate);
}

// =====================================================================
// Phase 1a - sync loads
// =====================================================================

void AssetManagerTest::runSyncLoads()
{
    STRIKE_INFO("--- SYNC LOADS ---");

    auto& am = Strike::AssetManager::get();

    mSyncModelObj   = am.load<Strike::Model>   (kIdSyncModelObj,   kPathSyncModelObj);
    mSyncModelFbx   = am.load<Strike::Model>   (kIdSyncModelFbx,   kPathSyncModelFbx);
    mSyncModelGltf  = am.load<Strike::Model>   (kIdSyncModelGltf,  kPathSyncModelGltf);
    mSyncTexturePng = am.load<Strike::Texture> (kIdSyncTexturePng, kPathTexturePng);
    mSyncTextureJpg = am.load<Strike::Texture> (kIdSyncTextureJpg, kPathTextureJpg);
    mSyncCubeMap    = am.load<Strike::CubeMap> (kIdSyncCubeMap,    kPathCubeMap);
    mSyncTemplateObj  = am.load<Strike::Template>(kIdSyncTemplateObj,  kPathTemplateObj);
    mSyncTemplateFbx  = am.load<Strike::Template>(kIdSyncTemplateFbx,  kPathTemplateFbx);
    mSyncTemplateGltf = am.load<Strike::Template>(kIdSyncTemplateGltf, kPathTemplateGltf);

    auto audioWav = am.load<Strike::Audio>(kIdSyncAudioWav, kPathAudioWav);
    auto audioMp3 = am.load<Strike::Audio>(kIdSyncAudioMp3, kPathAudioMp3);

    check("sync Audio WAV  isReady",      audioWav && audioWav->isReady());
    check("sync Audio MP3  isReady",      audioMp3 && audioMp3->isReady());
    check("sync Audio WAV  duration > 0", audioWav && audioWav->getDuration() > 0.0f);
    check("sync Audio MP3  duration > 0", audioMp3 && audioMp3->getDuration() > 0.0f);
}

// =====================================================================
// Phase 1b - immediate checks
// =====================================================================

void AssetManagerTest::runSyncImmediateTests()
{
    STRIKE_INFO("--- SYNC IMMEDIATE TESTS ---");

    auto& am = Strike::AssetManager::get();

    check("sync Model OBJ  handle valid",    mSyncModelObj   != nullptr);
    check("sync Model FBX  handle valid",    mSyncModelFbx   != nullptr);
    check("sync Model GLTF handle valid",    mSyncModelGltf  != nullptr);
    check("sync Texture PNG handle valid",   mSyncTexturePng != nullptr);
    check("sync Texture JPG handle valid",   mSyncTextureJpg != nullptr);
    check("sync CubeMap    handle valid",    mSyncCubeMap    != nullptr);
    check("sync Template OBJ  handle valid", mSyncTemplateObj  != nullptr);
    check("sync Template FBX  handle valid", mSyncTemplateFbx  != nullptr);
    check("sync Template GLTF handle valid", mSyncTemplateGltf != nullptr);

    check("sync Model OBJ  not failed",  mSyncModelObj   && !mSyncModelObj->hasFailed());
    check("sync Model FBX  not failed",  mSyncModelFbx   && !mSyncModelFbx->hasFailed());
    check("sync Model GLTF not failed",  mSyncModelGltf  && !mSyncModelGltf->hasFailed());
    check("sync Texture PNG not failed", mSyncTexturePng && !mSyncTexturePng->hasFailed());
    check("sync Texture JPG not failed", mSyncTextureJpg && !mSyncTextureJpg->hasFailed());
    check("sync CubeMap    not failed",  mSyncCubeMap    && !mSyncCubeMap->hasFailed());
    check("sync Template OBJ  not failed", mSyncTemplateObj  && !mSyncTemplateObj->hasFailed());
    check("sync Template FBX  not failed", mSyncTemplateFbx  && !mSyncTemplateFbx->hasFailed());
    check("sync Template GLTF not failed", mSyncTemplateGltf && !mSyncTemplateGltf->hasFailed());

    check("hasAsset sync_model_test1",     am.hasAsset(kIdSyncModelObj));
    check("hasAsset unknown returns false", !am.hasAsset("does_not_exist"));

    check("getAsset<Model> returns non-null",
          am.getAsset<Strike::Model>(kIdSyncModelObj) != nullptr);

    auto base = am.getAssetBase(kIdSyncTexturePng);
    check("getAssetBase returns non-null",    base != nullptr);
    check("getAssetBase typeName == texture", base && base->getTypeName() == "texture");
    check("getAssetBase path not empty",      base && !base->getPath().empty());
}

// =====================================================================
// Phase 1c - sync missing loads
// =====================================================================

void AssetManagerTest::runSyncMissingTests()
{
    STRIKE_INFO("--- SYNC MISSING ASSET LOADS ---");

    auto& am = Strike::AssetManager::get();

    mSyncMissingModel    = am.load<Strike::Model>   (kIdSyncMissingModel,    kPathMissingModel);
    mSyncMissingTexture  = am.load<Strike::Texture> (kIdSyncMissingTexture,  kPathMissingTexture);
    mSyncMissingAudio    = am.load<Strike::Audio>   (kIdSyncMissingAudio,    kPathMissingAudio);
    mSyncMissingCubeMap  = am.load<Strike::CubeMap> (kIdSyncMissingCubeMap,  kPathMissingCubeMap);
    mSyncMissingTemplate = am.load<Strike::Template>(kIdSyncMissingTemplate, kPathMissingTemplate);

    check("sync missing Audio hasFailed",   mSyncMissingAudio && mSyncMissingAudio->hasFailed());
    check("sync missing Audio not isReady", mSyncMissingAudio && !mSyncMissingAudio->isReady());

    check("sync missing Model    handle valid", mSyncMissingModel    != nullptr);
    check("sync missing Texture  handle valid", mSyncMissingTexture  != nullptr);
    check("sync missing CubeMap  handle valid", mSyncMissingCubeMap  != nullptr);
    check("sync missing Template handle valid", mSyncMissingTemplate != nullptr);
}

// =====================================================================
// Phase 1d - sync postLoad validation
// =====================================================================

void AssetManagerTest::runSyncPostLoadValidation()
{
    STRIKE_INFO("--- SYNC POST-LOAD VALIDATION ---");

    check("sync Model OBJ  isReady",    mSyncModelObj   && mSyncModelObj->isReady());
    check("sync Model FBX  isReady",    mSyncModelFbx   && mSyncModelFbx->isReady());
    check("sync Model GLTF isReady",    mSyncModelGltf  && mSyncModelGltf->isReady());
    check("sync Texture PNG isReady",   mSyncTexturePng && mSyncTexturePng->isReady());
    check("sync Texture JPG isReady",   mSyncTextureJpg && mSyncTextureJpg->isReady());
    check("sync CubeMap    isReady",    mSyncCubeMap    && mSyncCubeMap->isReady());
    check("sync Template OBJ  isReady", mSyncTemplateObj  && mSyncTemplateObj->isReady());
    check("sync Template FBX  isReady", mSyncTemplateFbx  && mSyncTemplateFbx->isReady());
    check("sync Template GLTF isReady", mSyncTemplateGltf && mSyncTemplateGltf->isReady());

    check("sync Model OBJ  getMeshCount",  mSyncModelObj  && mSyncModelObj->getMeshCount() > 0);
    check("sync Model FBX  getMeshCount",  mSyncModelFbx  && mSyncModelFbx->getMeshCount() > 0);
    check("sync Model GLTF getMeshCount",  mSyncModelGltf && mSyncModelGltf->getMeshCount() > 0);

    check("sync Texture PNG size > 0",     mSyncTexturePng && mSyncTexturePng->getWidth() > 0 && mSyncTexturePng->getHeight() > 0);
    check("sync Texture JPG size > 0",     mSyncTextureJpg && mSyncTextureJpg->getWidth() > 0 && mSyncTextureJpg->getHeight() > 0);
    check("sync Texture PNG channels > 0", mSyncTexturePng && mSyncTexturePng->getChannels() > 0);

    check("sync CubeMap size > 0", mSyncCubeMap && mSyncCubeMap->getWidth() > 0);

    check("sync Template OBJ  areAssetsReady", mSyncTemplateObj  && mSyncTemplateObj->areAssetsReady());
    check("sync Template FBX  areAssetsReady", mSyncTemplateFbx  && mSyncTemplateFbx->areAssetsReady());
    check("sync Template GLTF areAssetsReady", mSyncTemplateGltf && mSyncTemplateGltf->areAssetsReady());

    check("sync missing Model    hasFailed",   mSyncMissingModel    && mSyncMissingModel->hasFailed());
    check("sync missing Model    not isReady", mSyncMissingModel    && !mSyncMissingModel->isReady());
    check("sync missing Texture  hasFailed",   mSyncMissingTexture  && mSyncMissingTexture->hasFailed());
    check("sync missing Texture  not isReady", mSyncMissingTexture  && !mSyncMissingTexture->isReady());
    check("sync missing CubeMap  hasFailed",   mSyncMissingCubeMap  && mSyncMissingCubeMap->hasFailed());
    check("sync missing CubeMap  not isReady", mSyncMissingCubeMap  && !mSyncMissingCubeMap->isReady());
    check("sync missing Template hasFailed",   mSyncMissingTemplate && mSyncMissingTemplate->hasFailed());
    check("sync missing Template not isReady", mSyncMissingTemplate && !mSyncMissingTemplate->isReady());
}

// =====================================================================
// Phase 2 - async kickoff
// =====================================================================

void AssetManagerTest::runAsyncKickoff()
{
    STRIKE_INFO("--- ASYNC LOADS STARTED ---");

    auto& am = Strike::AssetManager::get();

    mAsyncModelObj   = am.loadAsync<Strike::Model>   (kIdAsyncModelObj,   kPathAsyncModelObj);
    mAsyncModelFbx   = am.loadAsync<Strike::Model>   (kIdAsyncModelFbx,   kPathAsyncModelFbx);
    mAsyncModelGltf  = am.loadAsync<Strike::Model>   (kIdAsyncModelGltf,  kPathAsyncModelGltf);
    mAsyncTexturePng = am.loadAsync<Strike::Texture> (kIdAsyncTexturePng, kPathTexturePng);
    mAsyncTextureJpg = am.loadAsync<Strike::Texture> (kIdAsyncTextureJpg, kPathTextureJpg);
    mAsyncAudioWav   = am.loadAsync<Strike::Audio>   (kIdAsyncAudioWav,   kPathAudioWav);
    mAsyncAudioMp3   = am.loadAsync<Strike::Audio>   (kIdAsyncAudioMp3,   kPathAudioMp3);
    mAsyncCubeMap    = am.loadAsync<Strike::CubeMap> (kIdAsyncCubeMap,    kPathCubeMap);
    mAsyncTemplateObj  = am.loadAsync<Strike::Template>(kIdAsyncTemplateObj,  kPathTemplateObj);
    mAsyncTemplateFbx  = am.loadAsync<Strike::Template>(kIdAsyncTemplateFbx,  kPathTemplateFbx);
    mAsyncTemplateGltf = am.loadAsync<Strike::Template>(kIdAsyncTemplateGltf, kPathTemplateGltf);

    mAsyncMissingModel    = am.loadAsync<Strike::Model>   (kIdAsyncMissingModel,    kPathMissingModel);
    mAsyncMissingTexture  = am.loadAsync<Strike::Texture> (kIdAsyncMissingTexture,  kPathMissingTexture);
    mAsyncMissingAudio    = am.loadAsync<Strike::Audio>   (kIdAsyncMissingAudio,    kPathMissingAudio);
    mAsyncMissingCubeMap  = am.loadAsync<Strike::CubeMap> (kIdAsyncMissingCubeMap,  kPathMissingCubeMap);
    mAsyncMissingTemplate = am.loadAsync<Strike::Template>(kIdAsyncMissingTemplate, kPathMissingTemplate);

    check("async Model OBJ   starts Loading",   mAsyncModelObj     && mAsyncModelObj->isLoading());
    check("async Model FBX   starts Loading",   mAsyncModelFbx     && mAsyncModelFbx->isLoading());
    check("async Model GLTF  starts Loading",   mAsyncModelGltf    && mAsyncModelGltf->isLoading());
    check("async Texture PNG starts Loading",   mAsyncTexturePng   && mAsyncTexturePng->isLoading());
    check("async Texture JPG starts Loading",   mAsyncTextureJpg   && mAsyncTextureJpg->isLoading());
    check("async Audio WAV   starts Loading",   mAsyncAudioWav     && mAsyncAudioWav->isLoading());
    check("async Audio MP3   starts Loading",   mAsyncAudioMp3     && mAsyncAudioMp3->isLoading());
    check("async CubeMap     starts Loading",   mAsyncCubeMap      && mAsyncCubeMap->isLoading());
    check("async Template OBJ  starts Loading", mAsyncTemplateObj  && mAsyncTemplateObj->isLoading());
    check("async Template FBX  starts Loading", mAsyncTemplateFbx  && mAsyncTemplateFbx->isLoading());
    check("async Template GLTF starts Loading", mAsyncTemplateGltf && mAsyncTemplateGltf->isLoading());

    check("async missing Model    starts Loading", mAsyncMissingModel    && mAsyncMissingModel->isLoading());
    check("async missing Texture  starts Loading", mAsyncMissingTexture  && mAsyncMissingTexture->isLoading());
    check("async missing Audio    starts Loading", mAsyncMissingAudio    && mAsyncMissingAudio->isLoading());
    check("async missing CubeMap  starts Loading", mAsyncMissingCubeMap  && mAsyncMissingCubeMap->isLoading());
    check("async missing Template starts Loading", mAsyncMissingTemplate && mAsyncMissingTemplate->hasFailed());

    check("isLoading() returns true after kickoff", am.isLoading());
}

// =====================================================================
// Phase 3 - async validation
// =====================================================================

void AssetManagerTest::runAsyncValidation()
{
    STRIKE_INFO("--- ASYNC VALIDATION ---");

    auto& am = Strike::AssetManager::get();

    check("async Model OBJ  isReady",    mAsyncModelObj  && mAsyncModelObj->isReady());
    check("async Model FBX  isReady",    mAsyncModelFbx  && mAsyncModelFbx->isReady());
    check("async Model GLTF isReady",    mAsyncModelGltf && mAsyncModelGltf->isReady());
    check("async Texture PNG isReady",   mAsyncTexturePng && mAsyncTexturePng->isReady());
    check("async Texture JPG isReady",   mAsyncTextureJpg && mAsyncTextureJpg->isReady());
    check("async Audio WAV  isReady",    mAsyncAudioWav && mAsyncAudioWav->isReady());
    check("async Audio MP3  isReady",    mAsyncAudioMp3 && mAsyncAudioMp3->isReady());
    check("async CubeMap    isReady",    mAsyncCubeMap  && mAsyncCubeMap->isReady());
    check("async Template OBJ  isReady", mAsyncTemplateObj  && mAsyncTemplateObj->isReady());
    check("async Template FBX  isReady", mAsyncTemplateFbx  && mAsyncTemplateFbx->isReady());
    check("async Template GLTF isReady", mAsyncTemplateGltf && mAsyncTemplateGltf->isReady());

    check("async Model OBJ  getMeshCount > 0",  mAsyncModelObj  && mAsyncModelObj->getMeshCount() > 0);
    check("async Model FBX  getMeshCount > 0",  mAsyncModelFbx  && mAsyncModelFbx->getMeshCount() > 0);
    check("async Model GLTF getMeshCount > 0",  mAsyncModelGltf && mAsyncModelGltf->getMeshCount() > 0);
    check("async Texture PNG width > 0",        mAsyncTexturePng && mAsyncTexturePng->getWidth() > 0);
    check("async Texture JPG width > 0",        mAsyncTextureJpg && mAsyncTextureJpg->getWidth() > 0);
    check("async Audio WAV  duration > 0",      mAsyncAudioWav && mAsyncAudioWav->getDuration() > 0.0f);
    check("async Audio MP3  duration > 0",      mAsyncAudioMp3 && mAsyncAudioMp3->getDuration() > 0.0f);
    check("async Template OBJ  areAssetsReady", mAsyncTemplateObj  && mAsyncTemplateObj->areAssetsReady());
    check("async Template FBX  areAssetsReady", mAsyncTemplateFbx  && mAsyncTemplateFbx->areAssetsReady());
    check("async Template GLTF areAssetsReady", mAsyncTemplateGltf && mAsyncTemplateGltf->areAssetsReady());

    check("async missing Model    hasFailed",   mAsyncMissingModel    && mAsyncMissingModel->hasFailed());
    check("async missing Model    not isReady", mAsyncMissingModel    && !mAsyncMissingModel->isReady());
    check("async missing Texture  hasFailed",   mAsyncMissingTexture  && mAsyncMissingTexture->hasFailed());
    check("async missing Texture  not isReady", mAsyncMissingTexture  && !mAsyncMissingTexture->isReady());
    check("async missing Audio    hasFailed",   mAsyncMissingAudio    && mAsyncMissingAudio->hasFailed());
    check("async missing Audio    not isReady", mAsyncMissingAudio    && !mAsyncMissingAudio->isReady());
    check("async missing CubeMap  hasFailed",   mAsyncMissingCubeMap  && mAsyncMissingCubeMap->hasFailed());
    check("async missing CubeMap  not isReady", mAsyncMissingCubeMap  && !mAsyncMissingCubeMap->isReady());
    check("async missing Template hasFailed",   mAsyncMissingTemplate && mAsyncMissingTemplate->hasFailed());
    check("async missing Template not isReady", mAsyncMissingTemplate && !mAsyncMissingTemplate->isReady());

    check("isLoading() false after all done", !am.isLoading());

    auto ids = am.getLoadedAssetIds();
    bool foundObj  = false;
    bool foundFbx  = false;
    bool foundGltf = false;
    for (auto& id : ids)
    {
        if (id == kIdAsyncTemplateObj)  foundObj  = true;
        if (id == kIdAsyncTemplateFbx)  foundFbx  = true;
        if (id == kIdAsyncTemplateGltf) foundGltf = true;
    }
    check("getLoadedAssetIds contains async_template_obj",  foundObj);
    check("getLoadedAssetIds contains async_template_fbx",  foundFbx);
    check("getLoadedAssetIds contains async_template_gltf", foundGltf);
}

// =====================================================================
// Phase 4 - cleanup
// =====================================================================

void AssetManagerTest::runCleanupTests()
{
    STRIKE_INFO("--- CLEANUP TESTS ---");

    auto& am = Strike::AssetManager::get();

    am.removeAsset(kIdSyncModelObj);
    am.removeAsset(kIdSyncModelFbx);
    am.removeAsset(kIdSyncModelGltf);
    am.removeAsset(kIdSyncTexturePng);
    am.removeAsset(kIdSyncTextureJpg);
    am.removeAsset(kIdSyncAudioWav);
    am.removeAsset(kIdSyncAudioMp3);
    am.removeAsset(kIdSyncCubeMap);
    am.removeAsset(kIdSyncTemplateObj);
    am.removeAsset(kIdSyncTemplateFbx);
    am.removeAsset(kIdSyncTemplateGltf);
    am.removeAsset(kIdSyncMissingModel);
    am.removeAsset(kIdSyncMissingTexture);
    am.removeAsset(kIdSyncMissingAudio);
    am.removeAsset(kIdSyncMissingCubeMap);
    am.removeAsset(kIdSyncMissingTemplate);

    am.removeAsset(kIdAsyncModelObj);
    am.removeAsset(kIdAsyncModelFbx);
    am.removeAsset(kIdAsyncModelGltf);
    am.removeAsset(kIdAsyncTexturePng);
    am.removeAsset(kIdAsyncTextureJpg);
    am.removeAsset(kIdAsyncAudioWav);
    am.removeAsset(kIdAsyncAudioMp3);
    am.removeAsset(kIdAsyncCubeMap);
    am.removeAsset(kIdAsyncTemplateObj);
    am.removeAsset(kIdAsyncTemplateFbx);
    am.removeAsset(kIdAsyncTemplateGltf);
    am.removeAsset(kIdAsyncMissingModel);
    am.removeAsset(kIdAsyncMissingTexture);
    am.removeAsset(kIdAsyncMissingAudio);
    am.removeAsset(kIdAsyncMissingCubeMap);
    am.removeAsset(kIdAsyncMissingTemplate);

    check("after cleanup: getLoadedAssetCount == 0",  am.getLoadedAssetCount() == 0);
    check("after cleanup: getLoadingAssetCount == 0", am.getLoadingAssetCount() == 0);
    check("after cleanup: isLoading() == false",      !am.isLoading());
    check("after cleanup: hasAsset sync_model_test1 == false",  !am.hasAsset(kIdSyncModelObj));
    check("after cleanup: hasAsset async_model_test4 == false", !am.hasAsset(kIdAsyncModelObj));
    check("after cleanup: sync Model OBJ  use_count == 1", mSyncModelObj.use_count() == 1);
    check("after cleanup: async Model OBJ use_count == 1", mAsyncModelObj.use_count() == 1);
}

// =====================================================================
// Phase 5a - cancel kickoff
// =====================================================================

void AssetManagerTest::runCancelKickoff()
{
    STRIKE_INFO("--- CANCEL KICKOFF ---");

    auto& am = Strike::AssetManager::get();

    mCancelModel    = am.loadAsync<Strike::Model>   (kIdCancelModel,    kPathAsyncModelObj);
    mCancelTexture  = am.loadAsync<Strike::Texture> (kIdCancelTexture,  kPathTexturePng);
    mCancelAudio    = am.loadAsync<Strike::Audio>   (kIdCancelAudio,    kPathAudioWav);
    mCancelCubeMap  = am.loadAsync<Strike::CubeMap> (kIdCancelCubeMap,  kPathCubeMap);
    mCancelTemplate = am.loadAsync<Strike::Template>(kIdCancelTemplate, kPathTemplateObj);

    check("cancel Model   starts Loading",  mCancelModel    && mCancelModel->isLoading());
    check("cancel Texture starts Loading",  mCancelTexture  && mCancelTexture->isLoading());
    check("cancel Audio   starts Loading",  mCancelAudio    && mCancelAudio->isLoading());
    check("cancel CubeMap starts Loading",  mCancelCubeMap  && mCancelCubeMap->isLoading());
    check("cancel Template starts Loading", mCancelTemplate && mCancelTemplate->isLoading());

    am.removeAsset(kIdCancelModel);
    am.removeAsset(kIdCancelTexture);
    am.removeAsset(kIdCancelAudio);
    am.removeAsset(kIdCancelCubeMap);
    am.removeAsset(kIdCancelTemplate);

    check("cancel Model   not in manager",  !am.hasAsset(kIdCancelModel));
    check("cancel Texture not in manager",  !am.hasAsset(kIdCancelTexture));
    check("cancel Audio   not in manager",  !am.hasAsset(kIdCancelAudio));
    check("cancel CubeMap not in manager",  !am.hasAsset(kIdCancelCubeMap));
    check("cancel Template not in manager", !am.hasAsset(kIdCancelTemplate));
}

// =====================================================================
// Phase 5b - cancel validation
// =====================================================================

void AssetManagerTest::runCancelValidation()
{
    STRIKE_INFO("--- CANCEL VALIDATION ---");

    auto& am = Strike::AssetManager::get();

    check("after drain: cancel Model   not in manager",  !am.hasAsset(kIdCancelModel));
    check("after drain: cancel Texture not in manager",  !am.hasAsset(kIdCancelTexture));
    check("after drain: cancel Audio   not in manager",  !am.hasAsset(kIdCancelAudio));
    check("after drain: cancel CubeMap not in manager",  !am.hasAsset(kIdCancelCubeMap));
    check("after drain: cancel Template not in manager", !am.hasAsset(kIdCancelTemplate));

    check("after cancel: isLoading() == false",        !am.isLoading());
    check("after cancel: getLoadingAssetCount() == 0", am.getLoadingAssetCount() == 0);

    check("cancel Model   use_count == 1",  mCancelModel.use_count() == 1);
    check("cancel Texture use_count == 1",  mCancelTexture.use_count() == 1);
    check("cancel Audio   use_count == 1",  mCancelAudio.use_count() == 1);
    check("cancel CubeMap use_count == 1",  mCancelCubeMap.use_count() == 1);
    check("cancel Template use_count == 1", mCancelTemplate.use_count() == 1);
}

REGISTER_SCRIPT(AssetManagerTest)