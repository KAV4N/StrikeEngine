#pragma once

#include <vector>
#include <memory>
#include "Entity.h"
#include "StrikeEngine/Renderer/Shader.h"
#include "Camera.h"
#include "Light.h" 
#include <StrikeEngine/Renderer/Skybox.h>

namespace StrikeEngine {

    class Scene {
    public:
        Scene();
        ~Scene();

        void AddEntity(Entity* entity);
        void AddLight(Light light);
        void SetCamera(Camera* camera);
        void SetSkyboxModel(Model* model); 

        void Update();
        void Render();

        Camera* GetCamera();
        const std::vector<Light>& GetLights() const;
        Model* GetSkyboxModel() const;

    public:
        std::string m_Name = "default";

    private:
        std::vector<Entity*> m_Entities;
        std::vector<Light> m_Lights;
        Camera* m_Camera;
        Skybox* m_Skybox;
    };
}

