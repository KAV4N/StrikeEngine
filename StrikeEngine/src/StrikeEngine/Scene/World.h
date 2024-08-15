#pragma once

#include <vector>
#include <memory>
#include "Scene.h"

namespace StrikeEngine {

    class World {
    public:
        static void Create();
        static World* Get();

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        void AddScene(Scene* scene);
        void SetActiveScene(int index);

        void Update();
        void Render();

        Scene* GetActiveScene();

    private:
        World();
        ~World();

        static World* s_Instance;

        std::vector<Scene*> m_Scenes;
        Scene* m_ActiveScene;
    };

}
