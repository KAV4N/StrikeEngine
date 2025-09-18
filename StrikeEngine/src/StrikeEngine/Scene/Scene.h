#pragma once
#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include "StrikeEngine/Asset/Types/Asset.h"
#include "StrikeEngine/Core/LayerStack.h"
#include "StrikeEngine/Core/Layer.h"
#include "SceneGraph.h"
#include "Entity.h"

#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {
    class SceneGraph;
    class Entity;
    class Layer;

    class Scene final {
    public:
        Scene(const std::string& id, const std::filesystem::path& path, const std::string& name = "Untitled");
        ~Scene() = default;


        Entity getRootEntity() { return mSceneGraph->getRootEntity(); };
        void reset();

        // Update methods
        void onUpdate(float dt);
        void onRender();

        // Scene state
        void setActive(bool active) { mActive = active; }
        bool isActive() const { return mActive; }

        // Scene assets management
        const std::unordered_map<std::string, std::shared_ptr<Asset>>& getSceneAssets() const { return mSceneAssets; }
        std::unordered_map<std::string, std::shared_ptr<Asset>>& getSceneAssets() { return mSceneAssets; }
        void clearSceneAssets() { mSceneAssets.clear(); }

        SceneGraph* getSceneGraph() { return mSceneGraph.get(); }


    private:
        friend class Entity;
        friend class SceneLoader;

        std::unique_ptr<SceneGraph> mSceneGraph;
        std::unordered_map<std::string, std::shared_ptr<Asset>> mSceneAssets;

        bool mActive = true;
    };
}