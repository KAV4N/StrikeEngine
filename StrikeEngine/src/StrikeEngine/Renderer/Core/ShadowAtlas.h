#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

namespace StrikeEngine {
    class ShadowAtlas {
    public:
        ShadowAtlas(int atlasSize, int tileSize);
        ~ShadowAtlas();
        void Bind();
        void Unbind();
        glm::vec4 AllocateTile();
        GLuint GetTextureID() const { return m_TextureID; }

        void FreeTile(const glm::vec4& tile);
        bool InsertTile(const glm::vec4& tile, const void* data);
        void ClearAllTextures();
        void ResizeAtlas(int newAtlasSize);
        bool IsTileUsed(int index) const;

        int GetUsedTileCount() const { return m_UsedTileCount; }
        int GetFreeTileCount() const { return m_TotalTiles - m_UsedTileCount; }
        int GetTotalTileCount() const { return m_TotalTiles; }

    private:
        int GetTileIndex(const glm::vec4& tile) const;
    private:
        GLuint m_FBO;
        GLuint m_TextureID;
        int m_AtlasSize;
        int m_TileSize;
        int m_TilesPerSide;
        std::vector<bool> m_UsedTiles;

        int m_UsedTileCount;
        int m_TotalTiles;

    };
}