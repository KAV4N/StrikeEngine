#pragma once


#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <StrikeEngine/Graphics/Core/Shader.h>
#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Components/ModelComponent.h"

namespace StrikeEngine {
    struct RenderCommand {
        Entity cameraEntity;
        std::unordered_map<Shader*, std::vector<Entity>> shaderEntityMap;
        RenderCommand(entt::entity camera, Scene* scene) : cameraEntity(camera, scene) {};
    };
}

