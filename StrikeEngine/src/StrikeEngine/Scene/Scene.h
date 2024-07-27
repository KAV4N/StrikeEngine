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
        Entity* CreateEntity(Model* model, const glm::vec3& position = glm::vec3(0.0f),
            const glm::vec3& rotation = glm::vec3(0.0f),
            const glm::vec3& scale = glm::vec3(1.0f));


        void RemoveEntity(size_t index);




        void Setup();

        void SetCamera(Camera* camera);

        void Update();
        void Render();

        Camera* GetCamera();
        const std::vector<Light>& GetLights();

        friend class LightManager;

    public:
        std::string Name = "default";

    private:
        void AddLight(const Light& light);
        void RemoveLight(size_t index);
        void UpdateLight(size_t index, const Light& light);
        void ClearLights();
    private:
        std::vector<Entity*> m_Entities;
        std::vector<Light> m_Lights;
        Camera* m_Camera;
        Skybox* m_Skybox;
    };
}
