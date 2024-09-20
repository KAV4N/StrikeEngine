#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Graphics/Core/Shader.h"
#include "StrikeEngine/Graphics/Core/ShadowAtlas.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h"

namespace StrikeEngine {

    class ShadowRenderer {
    public:


        static void Create();
        static ShadowRenderer* Get();
        void UpdateShadowMaps(std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue);


        int GetAtlasSize();
        int GetTileSize();

        int GetTextureID();

    private:
        ShadowRenderer();  
        ~ShadowRenderer(); 

        void Render(std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue, const glm::mat4& viewProjectionMatrix);

        void UpdateDirectionalLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue);
        void UpdatePointLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue);
        void UpdateSpotLightShadowMap(const Entity& entity, std::unordered_map<Shader*, std::vector<RenderCommand>>& renderQueue);


    private:
        Shader* m_ShadowMapShader;
        std::unique_ptr<ShadowAtlas> m_ShadowAtlas;
        static ShadowRenderer* m_Instance;
    };
}
