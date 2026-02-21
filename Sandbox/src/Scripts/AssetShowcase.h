#pragma once
#include "StrikeEngine.h"

class AssetShowcase : public Strike::Script
{
public:
    void onStart() override;
    void onUpdate(float dt) override;

private:
    // Step machine
    void advance();
    void updateLabel(const std::string& text);
    void pollAsync();

    // Steps (clean names)
    void queryInitialState();
    void syncLoadModel();
    void asyncLoadModel();
    void syncLoadTexture();
    void asyncLoadTexture();
    void syncLoadCubeMap();
    void asyncLoadCubeMap();
    void asyncLoadTemplate();
    void syncLoadTemplate();
    void instantiateTemplate();
    void syncLoadAudio();
    void asyncLoadAudio();
    void queryAssetManager();
    void removeAssetDemo();
    void testHasAsset();
    void testIsAssetLoading();
    void testIsLoading();
    void testGetAssetBase();
    void testRemoveLoadingAsset();
    void testClearAssets();
    void finalQuery();

private:
    int  mStep = -1;

    bool        mWaitingAsync = false;
    std::string mAsyncWaitId;

    Strike::Entity mLabel;
    Strike::Entity mTemplateRoot;

    static constexpr const char* ID_MODEL_SYNC    = "showcase_model_sync";
    static constexpr const char* ID_MODEL_ASYNC   = "showcase_model_async";
    static constexpr const char* ID_TEX_SYNC      = "showcase_tex_sync";
    static constexpr const char* ID_TEX_ASYNC     = "showcase_tex_async";
    static constexpr const char* ID_TEMPLATE_SYNC = "showcase_template";
    static constexpr const char* ID_AUDIO_SYNC    = "showcase_audio_sync";
    static constexpr const char* ID_AUDIO_ASYNC   = "showcase_audio_async";
    static constexpr const char* ID_CUBEMAP_SYNC   = "showcase_cubemap_sync";
    static constexpr const char* ID_CUBEMAP_ASYNC  = "showcase_cubemap_async";
    static constexpr const char* ID_TEMPLATE_ASYNC = "showcase_template_async";
    static constexpr const char* ID_TEST_ASYNC     = "showcase_test_async";
};