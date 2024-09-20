
#pragma once

#include "StrikeEngine/Graphics/Core/Texture.h"
#include "StrikeEngine/Graphics/Core/Shader.h"
#include "StrikeEngine/Graphics/Managers/ShaderManager.h"
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



        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
        void Draw(Shader* shader);
    private:
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
        std::vector<Texture*> Textures;

        unsigned int VAO, VBO, EBO;
        void SetupMesh();
    };

    class Model {
    public:
        Model() = default;
        void AddMesh(Mesh* mesh);
        const std::vector<Mesh*>& GetMeshes() const;
    private:
        std::vector<Mesh*> MeshStorage;
    };
}
