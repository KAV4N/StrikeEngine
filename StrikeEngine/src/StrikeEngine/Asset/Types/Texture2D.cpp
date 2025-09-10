#include "Texture2D.h"
#include <iostream>

namespace StrikeEngine {

    Texture2D::Texture2D(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Asset(id, path, name),
        mData(nullptr),
        mWidth(0),
        mHeight(0),
        mChannels(0),
        mTextureID(0),
        mHasOpenGLResources(false),
        mFormat(TextureFormat::RGBA),
        mMinFilter(TextureFilter::Linear),
        mMagFilter(TextureFilter::Linear),
        mWrapS(TextureWrap::Repeat),
        mWrapT(TextureWrap::Repeat),
        mGenerateMipmaps(true)
    {
    }

    Texture2D::~Texture2D() {
        cleanup();
    }

    void Texture2D::postLoad() {
        createOpenGLTexture();
    }

    void Texture2D::bind(uint32_t slot) const {
        if (mHasOpenGLResources) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, mTextureID);
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

        // Auto-determine format based on channels
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

    void Texture2D::createOpenGLTexture() {
        if (!mData || mWidth == 0 || mHeight == 0) {
            std::cerr << "Cannot create OpenGL texture: invalid data for texture " << getId() << std::endl;
            return;
        }

        glGenTextures(1, &mTextureID);
        glBindTexture(GL_TEXTURE_2D, mTextureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(mMinFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mMagFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(mWrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(mWrapT));

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(mFormat), mWidth, mHeight, 0,
            static_cast<GLenum>(mFormat), GL_UNSIGNED_BYTE, mData);

        // Generate mipmaps if requested
        if (mGenerateMipmaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        mHasOpenGLResources = true;
        
        if (mData) {
            delete[] mData;
            mData = nullptr;
        }
        
        
    }

    void Texture2D::cleanup() {
        if (mHasOpenGLResources && mTextureID != 0) {
            glDeleteTextures(1, &mTextureID);
            mTextureID = 0;
            mHasOpenGLResources = false;
        }

        if (mData) {
            delete[] mData;
            mData = nullptr;
        }
    }

    pugi::xml_node Texture2D::toNode() const {
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName());
        node.append_attribute("assetId") = getId().c_str();
        node.append_attribute("src") = getPath().string().c_str();
        return node;
    }

}