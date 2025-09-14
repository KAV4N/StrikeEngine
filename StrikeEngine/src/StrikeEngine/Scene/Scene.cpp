#include "Scene.h"
#include "SceneGraph.h"
#include "Entity.h"
#include <random>
#include <sstream>
#include <iomanip>

namespace StrikeEngine {
    Scene::Scene(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : mSceneGraph(std::make_unique<SceneGraph>())
        , mActive(true)
    {
        createRootEntity(id, name);
    }

    void Scene::createRootEntity(const std::string& id, const std::string& name) {
        entt::entity handle = mRegistry.create();
        mRootEntity = Entity(*this, handle);
        mSceneGraph->createRootNode(mRootEntity, id, name);
    }

    Entity Scene::createEntity(const std::string& id, const std::string& name) {
        entt::entity handle = mRegistry.create();
        Entity entity(*this, handle);
        mSceneGraph->createNode(entity, id, name);
        // By default, parent new entities to the root
        mSceneGraph->setParent(entity, mRootEntity);
        return entity;
    }

    void Scene::destroyEntity(Entity entity) {
        if (!isEntityValid(entity)) {
            return;
        }
        // Prevent deletion of root entity
        if (isRootEntity(entity)) {
            return;
        }
        mSceneGraph->removeNode(entity);
        mRegistry.destroy(entity.handle);
    }

    bool Scene::isEntityValid(Entity entity) const {
        return mRegistry.valid(entity.handle);
    }

    Entity Scene::getEntity(entt::entity handle) {
        if (!mRegistry.valid(handle)) {
            return Entity(*this, entt::null);
        }
        return Entity(*this, handle);
    }

    Entity Scene::getEntityByName(const std::string& name) {
        return mSceneGraph->getEntityByName(name);
    }

    Entity Scene::getEntityById(const std::string& id) {
        return mSceneGraph->getEntityById(id);
    }

    Entity Scene::getRootEntity() const {
        return mRootEntity;
    }

    bool Scene::isRootEntity(Entity entity) const {
        return entity.isValid() && entity == mRootEntity;
    }

    void Scene::reset() {
        std::string entId = mRootEntity.getId();
        std::string entName = mRootEntity.getName();

        mSceneGraph->clear();
        mRegistry.clear();
        createRootEntity(entId, entName);
    }

    void Scene::onUpdate(float dt) {
        if (!mActive) return;

        // Visual/rendering updates - interpolation, animations, etc.
        mSceneGraph->updateTransforms();
    }

    void Scene::onRender() {
        if (!mActive) return;

        // Rendering operations
        // - Submit render commands
        // - Prepare render data
        // - Culling operations
        // etc.
    }


    void Scene::onImGuiRender() {
        if (!mActive) return;

    }

    void Scene::onEvent(Event& e) {
        if (!mActive) return;
    }
}