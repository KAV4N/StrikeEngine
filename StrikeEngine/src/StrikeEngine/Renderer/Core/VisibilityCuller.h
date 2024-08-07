#pragma once
#include "StrikeEngine/Renderer/Core/Model.h"

namespace StrikeEngine {

    class VisibilityCuller {
    public:
        static bool IsVisible(const ModelPart::AABB& aabb, const glm::mat4& modelMatrix, const glm::mat4& viewProjectionMatrix);
    };

}