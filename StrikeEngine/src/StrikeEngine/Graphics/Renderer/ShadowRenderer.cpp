#include "strikepch.h"
#include "ShadowRenderer.h"

#include <StrikeEngine/Scene/Components/ShadowCasterComponent.h>
#include "StrikeEngine/Scene/Components/LightComponents.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"

#include "StrikeEngine/Graphics/Core/VisibilityCuller.h"
#include "StrikeEngine/Scene/World.h"

namespace StrikeEngine {

    ShadowRenderer* ShadowRenderer::m_Instance = nullptr;

    ShadowRenderer::ShadowRenderer()
        : m_ShadowMapShader(ShaderManager::Get()->GetShader("ShadowMapShader")),
        m_ShadowAtlas(std::make_unique<ShadowAtlas>(2048, 1024))
    {
    }


    ShadowRenderer::~ShadowRenderer() {

        if (m_ShadowMapShader) {
            delete m_ShadowMapShader;
            m_ShadowMapShader = nullptr;
        }
    }

    void ShadowRenderer::Create()
    {
        if (!m_Instance)
        {
            m_Instance = new ShadowRenderer();
        }
    }

    ShadowRenderer* ShadowRenderer::Get()
    {
        return m_Instance;
    }

    void ShadowRenderer::UpdateShadowMaps(std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue)
    {
        
        auto& registry = World::Get()->GetActiveScene()->GetRegistry();

        auto pointView = registry.view<PointLightComponent, ShadowCasterComponent>();
        auto dirView = registry.view<DirectionalLightComponent, ShadowCasterComponent>();
        auto spotView = registry.view<SpotLightComponent, ShadowCasterComponent>();

        m_ShadowAtlas->Bind();
        m_ShadowMapShader->Bind();

        for (auto entity : dirView) {
            UpdateDirectionalLightShadowMap({ entity, World::Get()->GetActiveScene() }, renderQueue);
        }

        for (auto entity : spotView) {
            UpdateSpotLightShadowMap({ entity, World::Get()->GetActiveScene() }, renderQueue);
        }

        for (auto entity : pointView) {
            UpdatePointLightShadowMap({ entity, World::Get()->GetActiveScene() }, renderQueue);
        }

        m_ShadowMapShader->Unbind();
        m_ShadowAtlas->Unbind();
        
    }

    void ShadowRenderer::Render(std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue, const glm::mat4& viewProjectionMatrix)
    {
        /*
        for (auto& pair : renderQueue) {
            for (const auto& command : pair.second) {
                const auto& modelComp = command.entity.GetComponent<ModelComponent>();
                for (const auto& partComp : modelComp.parts) {
                    glm::mat4 partTransform = command.transformationMatrix * partComp.localTransform;
                    if (VisibilityCuller::IsVisible(partComp.part->GetAABB(), partTransform, viewProjectionMatrix)) {
                        m_ShadowMapShader->LoadUniform("transform", partTransform);
                        partComp.part->Draw();
                    }
                }
            }
        }
        */
    }

    void ShadowRenderer::UpdateDirectionalLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue) {
        /*
        auto& light = entity.GetComponent<DirectionalLightComponent>();
        auto& shadowCaster = entity.GetComponent<ShadowCasterComponent>();

        if (shadowCaster.atlasInfo == glm::vec4(-1)) {
            if (m_ShadowAtlas->GetFreeTileCount() > 0) {
                shadowCaster.atlasInfo = m_ShadowAtlas->AllocateTile();
            }
            else {
                return; // No free tiles available
            }
        }

        glm::vec4 tileInfo = shadowCaster.atlasInfo;
        glm::mat4 orthogonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.f);
        glm::mat4 lightView = glm::lookAt(light.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shadowCaster.lightSpaceMatrix = orthogonalProjection * lightView;
        m_ShadowMapShader->LoadUniform("lightSpaceMatrix", shadowCaster.lightSpaceMatrix);

        glEnable(GL_DEPTH_TEST);
        glViewport(tileInfo.x, tileInfo.y, tileInfo.z, tileInfo.w);
        glScissor(tileInfo.x, tileInfo.y, tileInfo.z, tileInfo.w);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        Render(renderQueue, shadowCaster.lightSpaceMatrix);
        glDisable(GL_SCISSOR_TEST);
        */
        
    }

    void ShadowRenderer::UpdateSpotLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue) {
      /*
        auto& light = entity.GetComponent<SpotLightComponent>();
        auto& shadowCaster = entity.GetComponent<ShadowCasterComponent>();

        if (shadowCaster.atlasInfo == glm::vec4(-1)) {
            if (m_ShadowAtlas->GetFreeTileCount() > 0) {
                shadowCaster.atlasInfo = m_ShadowAtlas->AllocateTile();
            }
            else {
                return; // No free tiles available
            }
        }

        glm::vec4 tileInfo = shadowCaster.atlasInfo;
        glm::mat4 lightProjection = glm::perspective(glm::radians(90.f), 1.0f, 0.1f, 1000.f);
        glm::mat4 lightView = glm::lookAt(light.position, light.position + light.direction, glm::vec3(0.0f, 1.0f, 0.0f));
        shadowCaster.lightSpaceMatrix = lightProjection * lightView;
        m_ShadowMapShader->LoadUniform("lightSpaceMatrix", shadowCaster.lightSpaceMatrix);

        glEnable(GL_DEPTH_TEST);
        glViewport(tileInfo.x, tileInfo.y, tileInfo.z, tileInfo.w);
        glScissor(tileInfo.x, tileInfo.y, tileInfo.z, tileInfo.w);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        Render(renderQueue, shadowCaster.lightSpaceMatrix);
        glDisable(GL_SCISSOR_TEST);
        */
    }

    void ShadowRenderer::UpdatePointLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue)
    {
        
    }

    int ShadowRenderer::GetAtlasSize() {
        return m_ShadowAtlas->GetAtlasSize();
    }

    int ShadowRenderer::GetTileSize() {
        return m_ShadowAtlas->GetTileSize();
    }

    int ShadowRenderer::GetTextureID() {
        return m_ShadowAtlas->GetTextureID();

    }

}
