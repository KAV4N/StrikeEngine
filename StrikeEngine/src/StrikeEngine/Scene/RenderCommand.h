#pragma once


#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <StrikeEngine/Renderer/Core/Shader.h>
#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Components/ModelComponent.h"

namespace StrikeEngine {
    struct RenderCommand {
        Entity cameraEntity;
        std::unordered_map<Shader*, std::vector<Entity>> shaderEntityMap;
        RenderCommand(entt::entity camera, Scene* scene) : cameraEntity(camera, scene) {};
    };

    /*
    struct MaterialData {
        glm::vec3 baseColor;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;

        MaterialData()
            : baseColor(1.0f, 1.0f, 1.0f), ambient(0.1f, 0.1f, 0.1f), diffuse(0.8f, 0.8f, 0.8f),
            specular(1.0f, 1.0f, 1.0f), shininess(32.0f) {}


        MaterialData(const glm::vec3& base, const glm::vec3& amb, const glm::vec3& diff,
            const glm::vec3& spec, float shine)
            : baseColor(base), ambient(amb), diffuse(diff), specular(spec), shininess(shine) {}
    };

    struct TextureData {
        GLuint DiffuseTextureID;
        GLuint SpecularTextureID;
        GLuint NormalTextureID;

        TextureData()
            : DiffuseTextureID(0), SpecularTextureID(0), NormalTextureID(0) {}


        TextureData(GLuint diffuseID, GLuint specularID, GLuint normalID)
            : DiffuseTextureID(diffuseID), SpecularTextureID(specularID), NormalTextureID(normalID) {}
    };

    struct TransformModelData {
        glm::mat4 ModelMatrix;

        TransformModelData()
            : ModelMatrix(glm::mat4(1.0f)) {}


        TransformModelData(const glm::mat4& modelMatrix)
            : ModelMatrix(modelMatrix) {}
    };

    struct ModelData {
        TransformModelData transformModelData;
        MaterialComponent materialData;
        TextureData textureData;


        ModelData() = default;


        ModelData(const TransformModelData& transform, const MaterialComponent& material, const TextureData& texture)
            : transformModelData(transform), materialData(material), textureData(texture) {}


        ModelData(const glm::mat4& modelMatrix)
            : transformModelData(modelMatrix) {}
    };

    struct CameraData {
        glm::vec3 Position;
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;

        CameraData()
            : Position(0.0f, 0.0f, 0.0f), ViewMatrix(glm::mat4(1.0f)), ProjectionMatrix(glm::mat4(1.0f)) {}

        CameraData(const glm::vec3& pos, const glm::mat4& viewMat, const glm::mat4& projMat)
            : Position(pos), ViewMatrix(viewMat), ProjectionMatrix(projMat) {}
    };

    struct Shader {};

    struct RenderCommand {
        CameraData camera;
        std::unordered_map<Shader*, std::vector<ModelData>> shaderBatches;

        RenderCommand() = default;

        RenderCommand(const CameraData& cam)
            : camera(cam) {}

        void Add(Shader* shader, const ModelData& transformData) {
            shaderBatches[shader].push_back(transformData);
        }
    };
    */
}

