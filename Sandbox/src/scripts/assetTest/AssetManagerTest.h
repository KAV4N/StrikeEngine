#pragma once
#include "StrikeEngine.h"


class AssetManagerTest : public Strike::Script
{
public:
    void onStart()   override;
    void onUpdate(float deltaTime) override;
    void onDestroy() override;

private:
    // Sync model IDs
    static constexpr const char* kIdSyncModelObj          = "sync_model_test1";
    static constexpr const char* kIdSyncModelFbx          = "sync_model_test2";
    static constexpr const char* kIdSyncModelGltf         = "sync_model_test3";

    // Sync other IDs
    static constexpr const char* kIdSyncTexturePng        = "sync_texture_png";
    static constexpr const char* kIdSyncTextureJpg        = "sync_texture_jpg";
    static constexpr const char* kIdSyncCubeMap           = "sync_cubemap";
    static constexpr const char* kIdSyncTemplateObj       = "sync_template_obj";
    static constexpr const char* kIdSyncTemplateFbx       = "sync_template_fbx";
    static constexpr const char* kIdSyncTemplateGltf      = "sync_template_gltf";
    static constexpr const char* kIdSyncAudioWav          = "sync_audio_wav";
    static constexpr const char* kIdSyncAudioMp3          = "sync_audio_mp3";

    // Sync missing IDs
    static constexpr const char* kIdSyncMissingModel      = "sync_missing_model";
    static constexpr const char* kIdSyncMissingTexture    = "sync_missing_texture";
    static constexpr const char* kIdSyncMissingAudio      = "sync_missing_audio";
    static constexpr const char* kIdSyncMissingCubeMap    = "sync_missing_cubemap";
    static constexpr const char* kIdSyncMissingTemplate   = "sync_missing_template";

    // Async model IDs
    static constexpr const char* kIdAsyncModelObj         = "async_model_test4";
    static constexpr const char* kIdAsyncModelFbx         = "async_model_test5";
    static constexpr const char* kIdAsyncModelGltf        = "async_model_test6";

    // Async other IDs
    static constexpr const char* kIdAsyncTexturePng       = "async_texture_png";
    static constexpr const char* kIdAsyncTextureJpg       = "async_texture_jpg";
    static constexpr const char* kIdAsyncAudioWav         = "async_audio_wav";
    static constexpr const char* kIdAsyncAudioMp3         = "async_audio_mp3";
    static constexpr const char* kIdAsyncCubeMap          = "async_cubemap";
    static constexpr const char* kIdAsyncTemplateObj      = "async_template_obj";
    static constexpr const char* kIdAsyncTemplateFbx      = "async_template_fbx";
    static constexpr const char* kIdAsyncTemplateGltf     = "async_template_gltf";

    // Async missing IDs
    static constexpr const char* kIdAsyncMissingModel     = "async_missing_model";
    static constexpr const char* kIdAsyncMissingTexture   = "async_missing_texture";
    static constexpr const char* kIdAsyncMissingAudio     = "async_missing_audio";
    static constexpr const char* kIdAsyncMissingCubeMap   = "async_missing_cubemap";
    static constexpr const char* kIdAsyncMissingTemplate  = "async_missing_template";

    // Cancel test IDs
    static constexpr const char* kIdCancelModel           = "cancel_model";
    static constexpr const char* kIdCancelTexture         = "cancel_texture";
    static constexpr const char* kIdCancelAudio           = "cancel_audio";
    static constexpr const char* kIdCancelCubeMap         = "cancel_cubemap";
    static constexpr const char* kIdCancelTemplate        = "cancel_template";

    // Sync model paths
    static constexpr const char* kPathSyncModelObj        = "Assets/Test/test1.obj";
    static constexpr const char* kPathSyncModelFbx        = "Assets/Test/test3.fbx";
    static constexpr const char* kPathSyncModelGltf       = "Assets/Test/test5.gltf";

    // Async model paths
    static constexpr const char* kPathAsyncModelObj       = "Assets/Test/test2.obj";
    static constexpr const char* kPathAsyncModelFbx       = "Assets/Test/test4.fbx";
    static constexpr const char* kPathAsyncModelGltf      = "Assets/Test/test6.gltf";

    // Template paths
    static constexpr const char* kPathTemplateObj         = "Assets/Test/test.obj";
    static constexpr const char* kPathTemplateFbx         = "Assets/Test/test.fbx";
    static constexpr const char* kPathTemplateGltf        = "Assets/Test/test.gltf";

    // Shared paths
    static constexpr const char* kPathTexturePng          = "Assets/Test/test.png";
    static constexpr const char* kPathTextureJpg          = "Assets/Test/test.jpg";
    static constexpr const char* kPathCubeMap             = "Assets/Test/skybox";
    static constexpr const char* kPathAudioWav            = "Assets/Test/test.wav";
    static constexpr const char* kPathAudioMp3            = "Assets/Test/test.mp3";

    // Missing paths
    static constexpr const char* kPathMissingModel        = "Assets/Test/missing.obj";
    static constexpr const char* kPathMissingTexture      = "Assets/Test/missing.png";
    static constexpr const char* kPathMissingAudio        = "Assets/Test/missing.wav";
    static constexpr const char* kPathMissingCubeMap      = "Assets/Test/missing_skybox";
    static constexpr const char* kPathMissingTemplate     = "Assets/Test/missing.obj";

    // Sync assets
    std::shared_ptr<Strike::Model>    mSyncModelObj;
    std::shared_ptr<Strike::Model>    mSyncModelFbx;
    std::shared_ptr<Strike::Model>    mSyncModelGltf;
    std::shared_ptr<Strike::Texture>  mSyncTexturePng;
    std::shared_ptr<Strike::Texture>  mSyncTextureJpg;
    std::shared_ptr<Strike::CubeMap>  mSyncCubeMap;
    std::shared_ptr<Strike::Template> mSyncTemplateObj;
    std::shared_ptr<Strike::Template> mSyncTemplateFbx;
    std::shared_ptr<Strike::Template> mSyncTemplateGltf;

    // Sync missing assets
    std::shared_ptr<Strike::Model>    mSyncMissingModel;
    std::shared_ptr<Strike::Texture>  mSyncMissingTexture;
    std::shared_ptr<Strike::Audio>    mSyncMissingAudio;
    std::shared_ptr<Strike::CubeMap>  mSyncMissingCubeMap;
    std::shared_ptr<Strike::Template> mSyncMissingTemplate;

    // Async assets
    std::shared_ptr<Strike::Model>    mAsyncModelObj;
    std::shared_ptr<Strike::Model>    mAsyncModelFbx;
    std::shared_ptr<Strike::Model>    mAsyncModelGltf;
    std::shared_ptr<Strike::Texture>  mAsyncTexturePng;
    std::shared_ptr<Strike::Texture>  mAsyncTextureJpg;
    std::shared_ptr<Strike::Audio>    mAsyncAudioWav;
    std::shared_ptr<Strike::Audio>    mAsyncAudioMp3;
    std::shared_ptr<Strike::CubeMap>  mAsyncCubeMap;
    std::shared_ptr<Strike::Template> mAsyncTemplateObj;
    std::shared_ptr<Strike::Template> mAsyncTemplateFbx;
    std::shared_ptr<Strike::Template> mAsyncTemplateGltf;

    // Async missing assets
    std::shared_ptr<Strike::Model>    mAsyncMissingModel;
    std::shared_ptr<Strike::Texture>  mAsyncMissingTexture;
    std::shared_ptr<Strike::Audio>    mAsyncMissingAudio;
    std::shared_ptr<Strike::CubeMap>  mAsyncMissingCubeMap;
    std::shared_ptr<Strike::Template> mAsyncMissingTemplate;

    // Cancel test handles
    std::shared_ptr<Strike::Model>    mCancelModel;
    std::shared_ptr<Strike::Texture>  mCancelTexture;
    std::shared_ptr<Strike::Audio>    mCancelAudio;
    std::shared_ptr<Strike::CubeMap>  mCancelCubeMap;
    std::shared_ptr<Strike::Template> mCancelTemplate;

    int  mPassCount = 0;
    int  mFailCount = 0;

    bool mSyncPostLoadValidated  = false;
    bool mAsyncValidated         = false;
    bool mCleanupValidated       = false;
    bool mCancelValidated        = false;

    void runSyncLoads();
    void runSyncImmediateTests();
    void runSyncPostLoadValidation();
    void runSyncMissingTests();
    void runAsyncKickoff();
    void runAsyncValidation();
    void runCleanupTests();
    void runCancelKickoff();
    void runCancelValidation();
    void printSummary();

    void check(const std::string& label, bool condition);
};