#pragma once

#include "StrikeEngine/Renderer/Core/Model.h"
#include "StrikeEngine/Renderer/Managers/ShaderManager.h"
#include <glm/glm.hpp>
#include <vector>

namespace StrikeEngine {

    struct MaterialComponent {
        glm::vec3 baseColor;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;

        MaterialComponent(
            glm::vec3 amb = glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3 dif = glm::vec3(0.8f, 0.8f, 0.8f),
            glm::vec3 spec = glm::vec3(1.0f, 1.0f, 1.0f),
            float shi = 32.0f
        ) :
            baseColor(glm::vec3(1.f)),
            ambient(amb),
            diffuse(dif),
            specular(spec),
            shininess(shi)
        {
        };
    };

    struct ShaderComponent {
        Shader* shader;
        ShaderComponent(Shader* shad = ShaderManager::Get()->GetShader(ShaderManager::Get()->GetDefaultShader())) : shader(shad)
        {};
    };

    struct TextureComponent {
        std::vector<Texture*> textures;
    };

    struct ModelPartComponent {
        unsigned int vaoID;
        unsigned int vertexCount;
        unsigned int vboID;
        unsigned int eboID;
    };


    struct ModelComponent {
        std::vector<Entity> parts;
    };
}
