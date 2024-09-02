
#pragma once

#include "StrikeEngine/Renderer/Core/Texture.h"
#include "StrikeEngine/Renderer/Core/Shader.h"
#include "StrikeEngine/Renderer/Managers/ShaderManager.h"
#include <vector>
#include <string>

namespace StrikeEngine {

    class Mesh {
    public:
        struct Vertex {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoords;
        };

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture*> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
        void Draw();
    private:
        unsigned int VAO, VBO, EBO;
        void SetupMesh();
    };

    class Model {
    public:
        Model() = default;
        void AddMesh(Mesh* mesh);
        const std::vector<Mesh*>& GetMeshes() const;
    private:
        std::vector<Mesh*> meshes;
    };
}
