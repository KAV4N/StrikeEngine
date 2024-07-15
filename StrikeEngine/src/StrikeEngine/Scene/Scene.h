#pragma once
#include <vector>
#include <memory>
#include "Entity.h"
#include "StrikeEngine/Renderer/Shader.h"
#include "Camera.h"

namespace StrikeEngine {
    class Scene {
    public:
        Scene();
        ~Scene();

        void AddEntity(Entity* entity);
        void SetCamera(Camera* camera);

        void Update();
        void Render(Shader* shader);

        Camera* GetCamera() const;

    private:
        std::vector<Entity*> m_Entities;
        Camera* m_Camera;
    };
}
