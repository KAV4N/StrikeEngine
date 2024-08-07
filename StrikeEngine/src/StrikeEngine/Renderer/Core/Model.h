#pragma once

#include "StrikeEngine/Renderer/Core/Texture.h"
#include "StrikeEngine/Renderer/Core/Shader.h"
#include "StrikeEngine/Renderer/Managers/ShaderManager.h"
#include <vector>
#include <algorithm> 

namespace StrikeEngine {


    struct Material {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    };


    const Material DEFAULT_MATERIAL = {
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(0.8f, 0.8f, 0.8f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        32.0f
    };

    class ModelPart {
    public:
        ModelPart(unsigned int vaoID, unsigned int vertexCount,
            unsigned int vboID, unsigned int eboID)
            : m_vaoID(vaoID), m_VertexCount(vertexCount),
            m_vboID(vboID), m_eboID(eboID)
        {}

        ~ModelPart()
        {
            for (Texture* texture : m_Textures)
            {
                delete texture;
            }
        }

        inline void AddTextures(const std::vector<Texture*>& textures) { m_Textures.insert(m_Textures.end(), textures.begin(), textures.end()); }

        void AddTexture(Texture* texture)
        {
            if (texture)
            {
                m_Textures.push_back(texture);
            }
        }

        void AddTextureAt(size_t index, Texture* texture)
        {
            if (texture)
            {
                if (index > m_Textures.size())
                {
                    index = m_Textures.size();
                }
                m_Textures.insert(m_Textures.begin() + index, texture);
            }
        }

        Texture* GetTexture(size_t index) const
        {
            if (index < m_Textures.size())
            {
                return m_Textures[index];
            }
            return nullptr;
        }

        bool GetTexture(Texture* textureToFind, size_t& index) const
        {
            auto it = std::find(m_Textures.begin(), m_Textures.end(), textureToFind);
            if (it != m_Textures.end())
            {
                index = std::distance(m_Textures.begin(), it);
                return true;
            }
            return false;
        }

        inline unsigned int GetVaoID() const { return m_vaoID; }
        inline unsigned int GetVertexCount() const { return m_VertexCount; }
        inline const std::vector<Texture*>& GetTextures() const { return m_Textures; }
        inline unsigned int GetVboID() const { return m_vboID; }
        inline unsigned int GetEboID() const { return m_eboID; }

        inline const Material& GetMaterial() const { return m_Material; }
        inline void SetMaterial(const Material& material) { m_Material = material; }

        bool ReplaceTexture(Texture* oldTexture, Texture* newTexture) {
            auto it = std::find(m_Textures.begin(), m_Textures.end(), oldTexture);
            if (it != m_Textures.end()) {
                delete* it;
                *it = newTexture;
                return true;
            }
            return false;
        }

        bool ReplaceTextureAt(size_t index, Texture* newTexture)
        {
            if (index < m_Textures.size())
            {
                delete m_Textures[index];
                m_Textures[index] = newTexture;
                return true;
            }
            return false;
        }

        bool DeleteTexture(Texture* textureToRemove)
        {
            auto it = std::find(m_Textures.begin(), m_Textures.end(), textureToRemove);
            if (it != m_Textures.end())
            {
                delete* it;
                m_Textures.erase(it);
                return true;
            }
            return false;
        }

        void Draw() {
            glBindVertexArray(m_vaoID);
            glDrawElements(GL_TRIANGLES, m_VertexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        struct AABB {
            glm::vec3 min;
            glm::vec3 max;
        };

        void SetAABB(const AABB& aabb) { m_AABB = aabb; }
        const AABB& GetAABB() const { return m_AABB; }

    private:
        unsigned int m_vaoID;
        unsigned int m_VertexCount;
        std::vector<Texture*> m_Textures;
        unsigned int m_vboID;
        unsigned int m_eboID;
        Material m_Material;
        AABB m_AABB;
    };

    class Model {
    public:
        Model(Shader* shader = nullptr)
            : m_Shader(shader ? shader : ShaderManager::Get()->GetShader(DEFAULT_SHADER)) {}

        void AddPart(ModelPart* part) {
            m_Parts.push_back(part);
        }

        inline const std::vector<ModelPart*>& GetParts() const { return m_Parts; }
        inline Shader* GetShader() const { return m_Shader; }
        inline void SetShader(Shader* shader) { m_Shader = shader; }

    private:
        std::vector<ModelPart*> m_Parts;
        Shader* m_Shader;
    };

}
