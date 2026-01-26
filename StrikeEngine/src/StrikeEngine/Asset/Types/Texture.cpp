#include "strikepch.h"
#include "Texture.h"

namespace StrikeEngine {

    // ============ Texture ============

    Texture::Texture(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path)
        , mTextureID(0)
        , mWidth(0)
        , mHeight(0)
        , mChannels(0)
    {
        mNeedsPostLoad = true;
    }

    Texture::~Texture() {
        if (mTextureID != 0) {
            glDeleteTextures(1, &mTextureID);
        }
    }
    
   
    void Texture::setTextureData(int width, int height, int channels, unsigned char* data) {
        mWidth = width;
        mHeight = height;
        mChannels = channels;
        
        if (data) {
            size_t dataSize = static_cast<size_t>(width) * height * channels;
            mData.resize(dataSize);
            std::memcpy(mData.data(), data, dataSize);
        }
    }

    void Texture::postLoad() {
        if (mTextureID != 0) {
            return; // Already loaded
        }

        if (mData.empty() || mWidth == 0 || mHeight == 0) {
            STRIKE_CORE_ERROR("Cannot create texture: no data loaded");
            setLoadingState(AssetState::Failed);
            return;
        }

        glGenTextures(1, &mTextureID);
        glBindTexture(GL_TEXTURE_2D, mTextureID);

        GLenum format = GL_RGB;
        if (mChannels == 1) format = GL_RED;
        else if (mChannels == 3) format = GL_RGB;
        else if (mChannels == 4) format = GL_RGBA;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, mData.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        mData.clear();
        mData.shrink_to_fit();
    }

    void Texture::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, mTextureID);
    }

    void Texture::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // ============ CubeMap ============

    CubeMap::CubeMap(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path)
        , mTextureID(0)
        , mWidth(0)
        , mHeight(0)
        , mChannels(0)
    {
        mNeedsPostLoad = true;
    }

    CubeMap::~CubeMap() {
        if (mTextureID != 0) {
            glDeleteTextures(1, &mTextureID);
        }
    }



    void CubeMap::setCubeMapData(int width, int height, int channels, unsigned char* faceData[6]) {
        mWidth = width;
        mHeight = height;
        mChannels = channels;

        if (faceData) {
            size_t faceSize = static_cast<size_t>(width) * height * channels;
            for (int i = 0; i < 6; i++) {
                if (faceData[i]) {
                    mFaceData[i].resize(faceSize);
                    std::memcpy(mFaceData[i].data(), faceData[i], faceSize);
                }
            }
        }
    }

    void CubeMap::postLoad() {
        if (mTextureID != 0) {
            return; // Already loaded
        }
        
        bool hasValidData = true;
        for (const auto& face : mFaceData) {
            if (face.empty()) {
                hasValidData = false;
                break;
            }
        }

        if (!hasValidData || mWidth == 0 || mHeight == 0) {
            STRIKE_CORE_ERROR("Cannot create cubemap: incomplete face data");
            setLoadingState(AssetState::Failed);
            return;
        }

        glGenTextures(1, &mTextureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

        GLenum format = GL_RGB;
        if (mChannels == 1) format = GL_RED;
        else if (mChannels == 3) format = GL_RGB;
        else if (mChannels == 4) format = GL_RGBA;

        for (int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
                        mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, mFaceData[i].data());
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        for (auto& face : mFaceData) {
            face.clear();
            face.shrink_to_fit();
        }
    }

    void CubeMap::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);
    }

    void CubeMap::unbind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

}