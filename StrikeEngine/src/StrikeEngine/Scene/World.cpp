#include "strikepch.h"
#include "World.h"

#include "Scene.h"
#include "SceneLoader.h"
#include "StrikeEngine/Events/Event.h"
#include "Systems/RenderSystem.h"
#include "Systems/ScriptSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/AudioSystem.h"

#include "StrikeEngine/Graphics/Skybox.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"

#include "StrikeEngine/Asset/AssetManager.h"

namespace Strike {

    World& World::get() {
        static World instance;
        return instance;
    }

    World::World()
        : mSceneLoader(std::make_unique<SceneLoader>())
        , mRenderSystem(std::make_unique<RenderSystem>())
        , mScriptSystem(std::make_unique<ScriptSystem>())
        , mPhysicsSystem(std::make_unique<PhysicsSystem>())
        , mAudioSystem(std::make_unique<AudioSystem>())
    {}

    void World::loadScene(const std::filesystem::path& path, bool clearAssets) {
        if (!std::filesystem::exists(path)) {
            STRIKE_CORE_ERROR("Scene file does not exist: {}", path.string());
            return;
        }

        mPendingSceneData.clearAssets = clearAssets;
        mPendingSceneData.pendingScenePath = path;
        mPendingSceneData.mSceneLoadPending = true;
    }

    void World::processSceneLoad() {
        if (!mPendingSceneData.mSceneLoadPending) return;

        try {
            auto newScene = mSceneLoader->loadScene(mPendingSceneData.pendingScenePath, mPendingSceneData.clearAssets);
            if (newScene) {
                if (mCurrentScene) {
                    mAudioSystem->stopAll();
                    clearPhysicsWorld();
                    mCurrentScene->shutdown();
                }
                mCurrentScene = std::move(newScene);
            } else {
                STRIKE_CORE_ERROR("Failed to load scene: {}", mPendingSceneData.pendingScenePath.string());
            }
        } catch (const std::exception& e) {
            STRIKE_CORE_ERROR("Exception while loading scene: {}", e.what());
        }

        mPendingSceneData.mSceneLoadPending = false;
        mPendingSceneData.pendingScenePath.clear();
    }

    Scene* World::getScene() const {
        return mCurrentScene.get();
    }

    void World::onUpdate(float dt) {
        if (mCurrentScene) {
            mScriptSystem->onUpdate(dt);

            mCurrentScene->onUpdate(dt);
            mPhysicsSystem->onUpdate(dt);
            mCurrentScene->onUpdate(0.0f); // sync up the physycs changes

            mAudioSystem->onUpdate(dt);
            mRenderSystem->onUpdate(dt);
        }

        processSceneLoad();
    }

    RayHit World::rayCast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
        return mPhysicsSystem->rayCast(origin, direction, maxDistance);
    }

    std::vector<RayHit> World::rayCastAll(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
        return mPhysicsSystem->rayCastAll(origin, direction, maxDistance);
    }

    void World::setGravity(const glm::vec3& gravity) {
        mPhysicsSystem->setGravity(gravity);
    }

    glm::vec3 World::getGravity() const {
        return mPhysicsSystem->getGravity();
    }

    void World::onRender() {
        mRenderSystem->onRender();
    }

    void World::onEvent(Event& e) {
        if (mCurrentScene) {
            mScriptSystem->onEvent(e);
        }
    }

    void World::setFog(float start, float end, float density, const glm::uvec3& color) {
        mFog.start = start;
        mFog.end = end;
        mFog.density = density;
        mFog.color = color;

        Renderer::get().setFogStart(start);
        Renderer::get().setFogEnd(end);
        Renderer::get().setFogDensity(density);
        Renderer::get().setFogColor(color);
    }

    void World::resize(uint32_t width, uint32_t height) {
        mRenderSystem->resize(width, height);
    }

    void World::clearPhysicsWorld() {
        if (mPhysicsSystem && mCurrentScene) {
            auto& registry = mCurrentScene->getRegistry();
            auto view = registry.view<PhysicsComponent>();
            for (auto entity : view) {
                mPhysicsSystem->removePhysics(entity);
            }
        }
    }

    void World::shutDownSystems() {
        if (mAudioSystem) {
            mAudioSystem->stopAll();
        }
        if (mCurrentScene) {
            clearPhysicsWorld();
            mCurrentScene->shutdown();
        }
    }

}