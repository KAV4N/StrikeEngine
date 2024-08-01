#pragma once

#include <glm/glm.hpp>
#include <StrikeEngine/Renderer/FrameBuffer.h>

namespace StrikeEngine {

    struct ShadowData {
        glm::mat4 lightSpaceMatrix;
        int shadowMapIndex;
        float padding[3]; 
    };

    struct ShadowCasterComponent {
        std::unique_ptr<FrameBuffer> shadowMap;
        std::vector<glm::mat4> lightSpaceMatrices; 
        glm::mat4 lightSpaceMatrix;
        float shadowBias;
        float shadowIntensity;
        int shadowMapResolution;
        float nearPlane;
        float farPlane;

        ShadowCasterComponent(
            int resolution = 1024,
            float bias = 0.005f,
            float intensity = 1.0f,
            float near = 0.1f,
            float far = 100.0f)
            : shadowMap(std::make_unique<FrameBuffer>(resolution, resolution, true)),
            lightSpaceMatrix(glm::mat4(1.0f)),
            shadowBias(bias),
            shadowIntensity(intensity),
            shadowMapResolution(resolution),
            nearPlane(near),
            farPlane(far)
        {
            shadowMap->SetShadowMap(GL_DEPTH_COMPONENT24);
        }
    };
}
