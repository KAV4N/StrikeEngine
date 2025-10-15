#pragma once

#include "StrikeEngine/Graphics/Core/Model.h"
#include "StrikeEngine/Graphics/Managers/ShaderManager.h"
#include <glm/glm.hpp>
#include <vector>
#include <StrikeEngine/Graphics/Core/FrameBuffer.h>
#include <StrikeEngine/Scene/Camera.h>

namespace StrikeEngine {
    class Entity;

    struct RenderItem {
        Mesh* MeshObj;
        glm::mat4 Transform;
    };

    struct ActiveComponent {
        bool isActive = false; 
    };


    struct HierarchyComponent {
    private:
        friend class Entity;

        Entity parentEntity;
        Entity firstEntity;
        Entity previousEntity;
        Entity nextEntity;
        std::size_t childrenCount;
    };

    struct LabelComponent {
        std::string label = "default";
    };

    struct CameraComponent {
        unsigned int priority = 0;
        std::vector<std::string> renderLayerTags;

        std::shared_ptr<Camera> camera = std::make_shared<Camera>();
        std::shared_ptr<FrameBuffer> renderTarget;
    };

    struct RenderComponent {
        unsigned int priority = 0;
        std::vector<std::string> renderLayerTags;

        Mesh* mesh;
        Shader* shader;

        RenderComponent(Mesh* m, Shader* shad = ShaderManager::Get()->GetShader(ShaderManager::Get()->GetDefaultShader()), bool v = true)
            : mesh(m), shader(shad), priority(0)
        {
        }
    };

   
}