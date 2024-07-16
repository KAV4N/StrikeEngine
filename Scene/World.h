#pragma once
#include <vector>
#include <memory>
#include "Scene.h"

namespace StrikeEngine {
    class World {
    public:
        World();
        ~World();

        void AddScene(Scene* scene);
        void SetActiveScene(int index);

        void Update();
        void Render(Shader* shader);

        Scene* GetActiveScene();

    private:
        std::vector<Scene*> m_Scenes;
        Scene* m_ActiveScene;
    };
}
