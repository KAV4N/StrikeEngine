#pragma once

#include <glm/glm.hpp>
#include <StrikeEngine/Renderer/FrameBuffer.h>

namespace StrikeEngine {

    struct ShadowCasterComponent {
        glm::vec4 atlasInfo; // x, y: offset in atlas, z, w: size of tile
        glm::mat4 lightSpaceMatrix;
        std::vector<glm::mat4> lightSpaceMatrices; // For point lights
        float shadowBias;
        float shadowIntensity;
        float nearPlane;
        float farPlane;

        ShadowCasterComponent(
            float bias = 0.005f,
            float intensity = 1.0f,
            float near = 0.1f,
            float far = 100.0f)
            : atlasInfo(glm::vec4(-1)),
            lightSpaceMatrix(glm::mat4(1.0f)),
            shadowBias(bias),
            shadowIntensity(intensity),
            nearPlane(near),
            farPlane(far)
        {}
        /*
        // Helper function to get the UV coordinates in the shadow atlas
        glm::vec2 GetAtlasUV(const glm::vec2& shadowMapUV) const {
            return (shadowMapUV * atlasInfo.zw + atlasInfo.xy) / glm::vec2(4096, 4096); // Assuming 4096x4096 atlas size
        }*/
    };
}
