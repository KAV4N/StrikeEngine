#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "StrikeEngine/Scene/Light.h"
#include <StrikeEngine/Scene/Scene.h>

namespace StrikeEngine {

    class Shader;

    class LightManager {
    public:
        static void Create();
        static LightManager* Get();
        static void Destroy();

        void AddLight(const Light& light);
        void RemoveLight(size_t index);
        void UpdateLight(size_t index, const Light& light);
        void Clear();

        void BindLightsToShader();
        void SetActiveScene(Scene* activeScene);

    private:
        LightManager();
        ~LightManager();

        void CreateSSBO();
        void UpdateSSBO();

        static LightManager* s_Instance;

        //std::vector<Light> m_Lights;
        Scene* m_ActiveScene;
        unsigned int m_SSBO;
        bool m_IsDirty;

        const unsigned int LIGHT_BUFFER_BINDING = 0;
    };

} // namespace StrikeEngine