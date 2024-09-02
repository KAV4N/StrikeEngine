#pragma once

#include "StrikeEngine/Renderer/Core/Model.h"
#include "StrikeEngine/Renderer/Managers/ShaderManager.h"
#include <glm/glm.hpp>
#include <vector>

namespace StrikeEngine {

    struct MeshComponent {
        Shader* shader;
        Mesh* mesh;
        bool visible;

        MeshComponent(Mesh* m,  Shader* shad = ShaderManager::Get()->GetShader(ShaderManager::Get()->GetDefaultShader()), bool v = true)
            : mesh(m), visible(v), shader(shad)
        {

        }

    };

    struct ParentComponent {
        bool visible;
        Entity parent;

        ParentComponent(Entity par) 
            : parent(par), visible(true)

        {

        }

    };

    struct ChildrenComponent {
        std::vector<Entity> children;
    };
}
