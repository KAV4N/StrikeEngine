#include "Texture.h"
#include <iostream>

namespace StrikeEngine {

    Texture::Texture(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Asset(id, path, name),
        mRendererID(0),
        mFormat(TextureFormat::RGBA)
    {
    }

    Texture::~Texture() {
        cleanup();
    }

    void Texture::cleanup() {
        if (mRendererID != 0) {
            glDeleteTextures(1, &mRendererID);
            mRendererID = 0;
            setLoadingState(AssetLoadingState::Uninitialized);
        }
    }

    Texture2D::Texture2D(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Texture(id, path, name),
        mData(nullptr),
        mWidth(0),
        mHeight(0),
        mChannels(0)
    {
    }

    Texture2D::~Texture2D() {
        if (mData) {
            delete[] mData;
            mData = nullptr;
        }
    }

    void Texture2D::postLoad() {
        if (!mData || mWidth == 0 || mHeight == 0) {
            std::cerr << "Cannot create OpenGL texture: invalid data for texture " << getId() << std::endl;
            return;
        }

        glGenTextures(1, &mRendererID);
        glBindTexture(GL_TEXTURE_2D, mRendererID);

        // Default texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(mFormat), mWidth, mHeight, 0,
            static_cast<GLenum>(mFormat), GL_UNSIGNED_BYTE, mData);

        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (mData) {
            delete[] mData;
            mData = nullptr;
        }
    }

    void Texture2D::bind(uint32_t slot) const {
        if (mRendererID != 0) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, mRendererID);
        }
    }

    void Texture2D::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture2D::setTextureData(unsigned char* data, uint32_t width, uint32_t height, uint32_t channels) {
        mData = data;
        mWidth = width;
        mHeight = height;
        mChannels = channels;

        switch (channels) {
        case 1:
            mFormat = TextureFormat::RED;
            break;
        case 2:
            mFormat = TextureFormat::RG;
            break;
        case 3:
            mFormat = TextureFormat::RGB;
            break;
        case 4:
            mFormat = TextureFormat::RGBA;
            break;
        default:
            mFormat = TextureFormat::RGBA;
            break;
        }
    }

    pugi::xml_node Texture2D::toNode() const {
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName().c_str());
        node.append_attribute("assetId") = getId().c_str();
        node.append_attribute("src") = getPath().string().c_str();
        return node;
    }

    CubeMap::CubeMap(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Texture(id, path, name),
        mSize(0),
        mChannels(0)
    {
        for (int i = 0; i < 6; ++i) {
            mData[i] = nullptr;
        }
        mFormat = TextureFormat::RGB; // Default for cubemaps
    }

    CubeMap::~CubeMap() {
        for (int i = 0; i < 6; ++i) {
            if (mData[i]) {
                delete[] mData[i];
                mData[i] = nullptr;
            }
        }
    }

    void CubeMap::postLoad() {
        if (!mData[0] || mSize == 0) {
            std::cerr << "Cannot create OpenGL cubemap: invalid data for texture " << getId() << std::endl;
            return;
        }

        glGenTextures(1, &mRendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mRendererID);

        // Load all 6 faces
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, static_cast<GLint>(mFormat),
                mSize, mSize, 0, static_cast<GLenum>(mFormat), GL_UNSIGNED_BYTE, mData[i]);
        }

        // Default cubemap parameters (same as your skybox)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // Clean up data after creating GPU texture
        for (int i = 0; i < 6; ++i) {
            if (mData[i]) {
                delete[] mData[i];
                mData[i] = nullptr;
            }
        }
    }

    void CubeMap::bind(uint32_t slot) const {
        if (mRendererID != 0) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_CUBE_MAP, mRendererID);
        }
    }

    void CubeMap::unbind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void CubeMap::setTextureData(unsigned char* data[6], uint32_t size, uint32_t channels) {
        for (int i = 0; i < 6; ++i) {
            mData[i] = data[i];
        }
        mSize = size;
        mChannels = channels;

        // Set format based on channels
        switch (channels) {
        case 1:
            mFormat = TextureFormat::RED;
            break;
        case 2:
            mFormat = TextureFormat::RG;
            break;
        case 3:
            mFormat = TextureFormat::RGB;
            break;
        case 4:
            mFormat = TextureFormat::RGBA;
            break;
        default:
            mFormat = TextureFormat::RGB; // Default for cubemaps
            break;
        }
    }

    pugi::xml_node CubeMap::toNode() const {
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName().c_str());
        node.append_attribute("assetId") = getId().c_str();
        node.append_attribute("src") = getPath().string().c_str();
        return node;
    }
}