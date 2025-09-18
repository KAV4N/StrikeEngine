#include "Scene.h"
#include "SceneGraph.h"
#include "Entity.h"
#include "StrikeEngine/Scene/Components/ScriptComponent.h"
#include <random>
#include <sstream>
#include <iomanip>

namespace StrikeEngine {
    Scene::Scene(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : mSceneGraph(std::make_unique<SceneGraph>(this, id, name))
        , mActive(true)
    {
  
    }

    void Scene::reset() {
        mSceneGraph->clear();
    }

    void Scene::onUpdate(float dt) {
        if (!mActive) return;
        mSceneGraph->updateTransforms();
    }

    void Scene::onRender() {
        if (!mActive) return;
    }
}