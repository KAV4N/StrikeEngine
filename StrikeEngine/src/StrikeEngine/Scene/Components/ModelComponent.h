#pragma once

#include "StrikeEngine/Graphics/Core/Model.h"
#include "StrikeEngine/Graphics/Managers/ShaderManager.h"
#include <glm/glm.hpp>
#include <vector>

namespace StrikeEngine {

    struct RenderItem {
        Mesh* MeshObj;
        glm::mat4 Transform;
    };


    struct VisibleTag {
        bool IsVisible = true;
    };

    struct MeshComponent {
        Shader* ShaderObj;
        Mesh* MeshObj;

        MeshComponent(Mesh* m,  Shader* shad = ShaderManager::Get()->GetShader(ShaderManager::Get()->GetDefaultShader()), bool v = true)
            : MeshObj(m), ShaderObj(shad)
        {

        }

    };

    struct ParentComponent {
        Entity Parent;

        ParentComponent(Entity par) 
            : Parent(par)
        {
        }

    };

    struct ChildrenComponent {
        std::vector<Entity> Children;
    };
}
