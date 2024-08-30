#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer3D/Renderer.h" 
#include "StrikeEngine/Scene/Systems/TransformSystem.h"
#include "Components/ModelComponent.h"
#include "Components/ShadowCasterComponent.h"
#include "Components/LightComponents.h"
#include "Systems/CameraSystem.h"
#include "StrikeEngine/Scene/RenderCommand.h"

namespace StrikeEngine {

    RenderCommand Scene::BuildRenderCommand() const {
        RenderCommand command(m_CameraEntity.GetHandle(), const_cast<Scene*>(this));


        auto view = m_Registry.view<ModelComponent>();
        for (auto entityView : view) {
            Entity entity(entityView, const_cast<Scene*>(this));
            const auto& shaderComponent = entity.GetComponent<ShaderComponent>();
            command.shaderEntityMap[shaderComponent.shader].push_back(entity);
        }

        return command;
    }

    Scene::Scene() : m_CameraEntity(m_Registry.create(), this) {
        m_Skybox = std::make_unique<Skybox>();

        // Add components to the camera entity
        m_CameraEntity.AddComponent<TransformComponent>();
        m_CameraEntity.AddComponent<PositionComponent>(glm::vec3(0.0f, 6.0f, 4.0f)); // Initial position
        m_CameraEntity.AddComponent<RotationComponent>(glm::vec3(0.0f, 0.0f, 0.0f)); // Initial rotation (pitch)

        // Add and initialize the CameraComponent
        auto& camera = m_CameraEntity.AddComponent<CameraComponent>(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    }


    Scene::~Scene() {
        // Cleanup resources if needed
    }



    std::vector<Entity>  Scene::GetDirectionalLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<DirectionalLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));
            
        }
        return lights;
    }

    std::vector<Entity>  Scene::GetPointLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<PointLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));
        }
        return lights;
    }

    std::vector<Entity>  Scene::GetSpotLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<SpotLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));    
        }
        return lights;
    }

    std::vector<Entity> Scene::GetShadowCastingLights() const {
        std::vector<Entity> shadowCasters;
        auto view = m_Registry.view<ShadowCasterComponent>();
        for (auto entity : view) {
            shadowCasters.emplace_back(entity, const_cast<Scene*>(this));
        }
        return shadowCasters;
    }


    Entity Scene::CreateEntity(Model* model, const std::string& name) {
        Entity modelEntity(m_Registry.create(), this);
        modelEntity.AddComponent<TransformComponent>();
        modelEntity.AddComponent<PositionComponent>();
        modelEntity.AddComponent<RotationComponent>();
        modelEntity.AddComponent<ScaleComponent>();
        modelEntity.AddComponent<ShaderComponent>();

        auto& modelComponent = modelEntity.AddComponent<ModelComponent>();
        for (const auto& partData : model->GetParts()) {
            Entity partEntity(m_Registry.create(), this);

            partEntity.AddComponent<TransformComponent>();
            partEntity.AddComponent<PositionComponent>();
            partEntity.AddComponent<RotationComponent>();
            partEntity.AddComponent<ScaleComponent>();
            // Adding model part data
            auto& modelPartComponent = partEntity.AddComponent<ModelPartComponent>();
            modelPartComponent.vaoID = partData->GetVaoID();
            modelPartComponent.vertexCount = partData->GetVertexCount();
            modelPartComponent.vboID = partData->GetVboID();
            modelPartComponent.eboID = partData->GetEboID();

            // Adding material data
            auto& materialComponent = partEntity.AddComponent<MaterialComponent>();
            Material material = partData->GetMaterial();
            materialComponent.ambient = material.ambient;
            materialComponent.diffuse = material.diffuse;
            materialComponent.specular = material.specular;
            materialComponent.shininess = material.shininess;


            

            // Adding texture data
            auto& textureComponent = partEntity.AddComponent<TextureComponent>();
            textureComponent.textures = partData->GetTextures();

            modelComponent.parts.push_back(partEntity);
        }
        return modelEntity;
    }

    std::vector<Entity> Scene::GetAllEntitiesWithModelComponent() const {
        std::vector<Entity> entities;
        auto view = m_Registry.view<ModelComponent>();
        for (auto entity : view) {
            entities.emplace_back(entity, const_cast<Scene*>(this));
        }
        return entities;
    }



    void Scene::RemoveEntity(entt::entity entity) {
        m_Registry.destroy(entity);
    }

    Entity Scene::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<DirectionalLightComponent>(direction, color, intensity);
        return entity;
    }

    Entity Scene::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<PointLightComponent>(position, color, intensity, radius);
        return entity;
    }

    Entity Scene::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<SpotLightComponent>(position, direction, cutoff, color, intensity);
        return entity;
    }

    Entity Scene::CreateShadowCaster(Entity lightEntity)
    {
        if (!lightEntity.HasComponent<DirectionalLightComponent>() &&
            !lightEntity.HasComponent<PointLightComponent>() &&
            !lightEntity.HasComponent<SpotLightComponent>()) {
            throw std::runtime_error("Entity must have a light component to create a ShadowCasterComponent.");
        }

        lightEntity.AddComponent<ShadowCasterComponent>();
        return lightEntity;
    }


    void Scene::Setup() {
        // Scene setup code
    }

    void Scene::Update() {
        TransformSystem::Update(this);
        //CameraSystem::UpdateCamera(m_CameraEntity);
    }



    void Scene::Render() {
        Renderer* renderer = Renderer::Get();
        auto renderCommand = BuildRenderCommand();
        renderer->SubmitSkybox(m_Skybox.get());
        renderer->SubmitCommand(renderCommand);
        //m_CameraEntity.GetComponent<CameraComponent>().Position = glm::vec3(0.0f, 4.0f, 4.0f);
        //renderer->SubmitCommand(renderCommand);
    }

    Entity Scene::GetCameraEntity() const {
        return m_CameraEntity;
    }

}
