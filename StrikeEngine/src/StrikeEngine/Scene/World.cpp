#include "World.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h"
#include <iostream>
#include <chrono>

namespace StrikeEngine {

    World::World()
        : mSceneLoader(std::make_unique<SceneLoader>())
        , mRenderSystem(std::make_unique<RenderSystem>())
        , mScriptSystem(std::make_unique<ScriptSystem>())
        , mSkybox(std::make_unique<Skybox>())
    {
    }

    void World::loadScene(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) {
            std::cerr << "Scene file does not exist: " << path << std::endl;
            return;
        }

        mCurrentScene = mSceneLoader->loadScene(path);

        if (mCurrentScene) {
            std::cout << "Successfully loaded scene from: " << path << std::endl;
        }
        else {
            std::cerr << "Failed to load scene from: " << path << std::endl;
        }
    }

    void World::loadSceneAsync(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) {
            std::cerr << "Scene file does not exist: " << path << std::endl;
            return;
        }

        if (mPendingScene.valid()) {
            std::cout << "Cancelling previous async scene loading operation" << std::endl;
        }

        mPendingScene = mSceneLoader->loadSceneAsync(path);

        if (mPendingScene.valid()) {
            std::cout << "Started async loading of scene from: " << path << std::endl;
        }
        else {
            std::cerr << "Failed to start async loading of scene from: " << path << std::endl;
        }
    }

    bool World::isSceneLoading() const
    {
        return mPendingScene.valid() && mPendingScene.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
    }

    void World::update(float dt)
    {
        mSceneLoader->update();
        checkAndSwitchScene();
        if (mCurrentScene && mCurrentScene->isActive()) {
            mCurrentScene->onUpdate(dt);
            mScriptSystem->onUpdate(dt);
            mRenderSystem->onUpdate(dt);
        }
    }

    void World::onRender()
    {
        auto& renderer = Renderer::get();
        renderer.render();
    }

    void World::onImGuiRender()
    {
        if (mCurrentScene && mCurrentScene->isActive()) {
        }
    }

    void World::onEvent(Event& e)
    {
        if (mCurrentScene && mCurrentScene->isActive()) {
            mScriptSystem->onEvent(e);
        }
    }

    void World::checkAndSwitchScene()
    {
        if (mPendingScene.valid() && mPendingScene.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                auto newScene = mPendingScene.get();
                if (newScene) {
                    mCurrentScene = std::move(newScene);
                    std::cout << "Successfully switched to new scene" << std::endl;
                }
                else {
                    std::cerr << "Async scene loading failed" << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Exception during scene switching: " << e.what() << std::endl;
            }
        }
    }

}