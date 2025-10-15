#include "strikepch.h"
#include "ShadowAtlas.h"

namespace StrikeEngine {

    ShadowAtlas::ShadowAtlas(int atlasSize, int tileSize)
        : m_AtlasSize(atlasSize), m_TileSize(tileSize), m_UsedTileCount(0) {
        m_TilesPerSide = atlasSize / tileSize;
        m_TotalTiles = m_TilesPerSide * m_TilesPerSide;
        m_UsedTiles.resize(m_TotalTiles, false);

        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, atlasSize, atlasSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ShadowAtlas::~ShadowAtlas() {
        glDeleteFramebuffers(1, &m_FBO);
        glDeleteTextures(1, &m_TextureID);
    }

    void ShadowAtlas::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void ShadowAtlas::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glm::vec4 ShadowAtlas::AllocateTile() {
        for (int i = 0; i < m_UsedTiles.size(); ++i) {
            if (!m_UsedTiles[i]) {
                m_UsedTiles[i] = true;
                m_UsedTileCount++;
                int x = (i % m_TilesPerSide) * m_TileSize;
                int y = (i / m_TilesPerSide) * m_TileSize;
                return glm::vec4(x, y, m_TileSize, m_TileSize);
            }
        }
        return glm::vec4(-1); 
    }

    void ShadowAtlas::FreeTile(const glm::vec4& tile) {
        int index = GetTileIndex(tile);
        if (index >= 0 && index < m_UsedTiles.size()) {
            m_UsedTiles[index] = false;
            m_UsedTileCount--;
        }
    }

    bool ShadowAtlas::InsertTile(const glm::vec4& tile, const void* data) {
        int index = GetTileIndex(tile);
        if (index >= 0 && index < m_UsedTiles.size() && !m_UsedTiles[index]) {
            m_UsedTiles[index] = true;
            glBindTexture(GL_TEXTURE_2D, m_TextureID);
            glTexSubImage2D(GL_TEXTURE_2D, 0, tile.x, tile.y, m_TileSize, m_TileSize, GL_DEPTH_COMPONENT, GL_FLOAT, data);
            m_UsedTileCount++;
            return true;
        }
        return false;
    }

    void ShadowAtlas::ClearAllTextures() {
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        std::vector<float> clearData(m_AtlasSize * m_AtlasSize, 1.0f);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_AtlasSize, m_AtlasSize, GL_DEPTH_COMPONENT, GL_FLOAT, clearData.data());
        std::fill(m_UsedTiles.begin(), m_UsedTiles.end(), false);
        m_UsedTileCount = 0;
    }

    void ShadowAtlas::ResizeAtlas(int newAtlasSize) {
        // Recreate the texture with the new size
        glDeleteTextures(1, &m_TextureID);
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, newAtlasSize, newAtlasSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        // Update atlas properties
        m_AtlasSize = newAtlasSize;
        m_TilesPerSide = newAtlasSize / m_TileSize;
        m_TotalTiles = m_TilesPerSide * m_TilesPerSide;
        m_UsedTiles.resize(m_TotalTiles, false);
        m_UsedTileCount = 0;

        // Reapply texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // Reattach to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureID, 0);


    }

    bool ShadowAtlas::IsTileUsed(int index) const {
        if (index >= 0 && index < m_UsedTiles.size()) {
            return m_UsedTiles[index];
        }
        return false;
    }

    int ShadowAtlas::GetTileIndex(const glm::vec4& tile) const {
        int x = static_cast<int>(tile.x) / m_TileSize;
        int y = static_cast<int>(tile.y) / m_TileSize;
        return y * m_TilesPerSide + x;
    }

} 