#pragma once

#include <vector>
#include <memory>
#include "Entity.h"
#include "StrikeEngine/Renderer/Shader.h"
#include "Camera.h"
#include "Light.h" 

namespace StrikeEngine {

    class Scene {
    public:
        Scene();
        ~Scene();

        void AddEntity(Entity* entity);
        void AddLight(Light* light); 
        void SetCamera(Camera* camera);

        void Update();
        void Render(Shader* shader);

        Camera* GetCamera();
        const std::vector<Light*>& GetLights() const { return m_Lights; } // Accessor for lights

    public:
        std::string m_Name = "default";

    private:
        std::vector<Entity*> m_Entities;
        std::vector<Light*> m_Lights;
        Camera* m_Camera;
    };
}
