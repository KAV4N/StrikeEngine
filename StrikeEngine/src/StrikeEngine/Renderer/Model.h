#pragma once

#include "StrikeEngine/Renderer/Texture.h"
#include <vector>

namespace StrikeEngine {

    class ModelPart {
    public:
        ModelPart(unsigned int vaoID, unsigned int vertexCount,
            unsigned int vboID, unsigned int eboID)
            : m_vaoID(vaoID), m_VertexCount(vertexCount),
            m_vboID(vboID), m_eboID(eboID)
        {}

        inline void AddTextures(std::vector<Texture*> textures) { m_Textures.insert(m_Textures.end(), textures.begin(), textures.end()); }
        inline void AddTexture(Texture* texture) { m_Textures.push_back(texture); }

        inline unsigned int GetVaoID() const { return m_vaoID; }
        inline unsigned int GetVertexCount() const { return m_VertexCount; }
        inline const std::vector<Texture*>& GetTextures() const { return m_Textures; }
        inline unsigned int GetVboID() const { return m_vboID; }
        inline unsigned int GetEboID() const { return m_eboID; }

    private:
        unsigned int m_vaoID;
        unsigned int m_VertexCount;
        std::vector<Texture*> m_Textures;
        unsigned int m_vboID;
        unsigned int m_eboID;
    };

    class Model
    {
    public:
        Model() {}

        void AddPart(ModelPart* part) {
            m_Parts.push_back(part);
        }

        inline const std::vector<ModelPart*>& GetParts() const { return m_Parts; }

    private:
        std::vector<ModelPart*> m_Parts;
    };

}
