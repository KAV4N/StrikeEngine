#include "strikepch.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Scene.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h" 
#include "Components/ModelComponent.h"
#include "Components/ShadowCasterComponent.h"
#include "Components/LightComponents.h"
#include "StrikeEngine/Scene/RenderCommand.h"
#include "StrikeEngine/Scene/Systems/TransformSystem.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include <glm/gtx/matrix_decompose.hpp>
#include "Components/NativeScriptComponent.h"

namespace StrikeEngine {

    Scene::Scene()
        : m_SceneGraph(this, Entity(m_Registry.create(), this))
    {
        m_SceneGraph.GetRootNode()->entity.AddComponent<TransformComponent>();
        
        m_ActiveView = new ScreenPanel();
        m_ViewList.push_back(m_ActiveView);
        m_Skybox = std::make_unique<Skybox>();
    }

    Scene::~Scene() {
        m_ActiveView = nullptr;
        for (ScreenPanel* view : m_ViewList)
            delete view;
    }

    std::vector<Entity> Scene::GetDirectionalLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<DirectionalLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));
        }
        return lights;
    }

    std::vector<Entity> Scene::GetPointLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<PointLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));
        }
        return lights;
    }

    std::vector<Entity> Scene::GetSpotLights() const {
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

    Entity Scene::CreateEntity() {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<TransformComponent>();
        m_SceneGraph.CreateNode(entity);
        return entity;
    }

    void Scene::DestroyEntity(Entity entity) {
        m_SceneGraph.RemoveNode(entity);
    }

    Entity Scene::CreateEmptyEntity() {
        return Entity(m_Registry.create(), this);
    }

    Entity Scene::CreateRenderableEntity(Model* model, const std::string& name) {
        Entity parentEntity = CreateEntity();

        Shader* defaultShader = ShaderManager::Get()->GetShader(ShaderManager::Get()->GetDefaultShader());

        for (Mesh* mesh : model->GetMeshes()) {
            Entity childEntity = CreateEntity();
            childEntity.AddComponent<RenderComponent>(mesh, defaultShader);
            childEntity.SetParent(parentEntity);
        }

        return parentEntity;
    }



    Entity Scene::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<DirectionalLightComponent>(color, intensity);
        entity.AddComponent<TransformComponent>(glm::vec3(0.f), direction);
        m_SceneGraph.CreateNode(entity);
        return entity;
    }

    Entity Scene::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<PointLightComponent>(color, intensity, radius);
        entity.AddComponent<TransformComponent>(position);
        m_SceneGraph.CreateNode(entity);
        return entity;
    }

    Entity Scene::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<SpotLightComponent>(color, intensity, cutoff);
        entity.AddComponent<TransformComponent>(position, direction);
        m_SceneGraph.CreateNode(entity);
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



    void Scene::SubmitMesh(SceneNode* node, const glm::mat4& parentTransform) {
        if (!node) return;

        glm::mat4 worldTransform = parentTransform;
        if (node->entity && node->entity.HasComponent<TransformComponent>()) {
            glm::mat4 transform = node->entity.GetComponent<TransformComponent>().CalculateTransformMatrix();
            worldTransform = parentTransform * transform;
        }


        if (node->entity && node->entity.HasComponent<RenderComponent>()) {
            const auto& meshComp = node->entity.GetComponent<RenderComponent>();
            Mesh* mesh = meshComp.mesh;
            Shader* shader = meshComp.shader;
            Renderer::SubmitMesh(shader, { mesh, worldTransform });
        }
        /*
        SceneNode* child = node->firstChild;
        while (child) {
            SubmitMesh(child, worldTransform);
            child = child->nextSibling;
        }*/

        
        for (SceneNode* child : node->children) {
            SubmitMesh(child, worldTransform);
        }
    }
    void Scene::SubmitSkybox() {
        Renderer::SubmitSkybox(m_Skybox.get());
    }

    void Scene::OnUpdate(float t_DeltaTime) {
        auto view = m_Registry.view<NativeScriptComponent>();
        for (auto entity : view) {
            auto& nsc = view.get<NativeScriptComponent>(entity);
            for (auto& script : nsc.GetAllScripts()) {
                if (script->GetEntityState() == ScriptState::Uninitialized) {
                    script->StartScript(entity, this);
                }
                script->UpdateScript(t_DeltaTime);
            }
        }


        SubmitSkybox();
        SubmitMesh(m_SceneGraph.GetRootNode(), glm::mat4(1.0f));
    }

    void Scene::OnEvent(Event& event) {
        m_ActiveView->OnEvent(event);
    }

    void Scene::RenderScene(FrameBuffer* frameBuffer) {
        Renderer::Get()->Resize(m_ActiveView->GetFrameBuffer()->GetWidth(), m_ActiveView->GetFrameBuffer()->GetHeight());
        m_ActiveView->OnRender();

        Renderer::Resize(frameBuffer->GetWidth(), frameBuffer->GetHeight());
        frameBuffer->Bind();
        m_ActiveView->CompositeView();
        frameBuffer->Unbind();
    }

}