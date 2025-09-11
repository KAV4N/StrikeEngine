#pragma once
#include <glad/glad.h>
#include "Asset.h"

#include <string>
#include <filesystem>
#include <pugixml.hpp>

namespace StrikeEngine {

    class GraphicsAsset;

    enum class TextureFormat {
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        RED = GL_RED,
        RG = GL_RG
    };

    enum class TextureFilter {
        Linear = GL_LINEAR,
        Nearest = GL_NEAREST,
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST
    };

    enum class TextureWrap {
        Repeat = GL_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        MirroredRepeat = GL_MIRRORED_REPEAT
    };

    class Texture2D : public Asset {
    public:
        Texture2D(const std::string& id, const std::filesystem::path& path, const std::string& name = "");
        virtual ~Texture2D();

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "texture2D";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void postload() override;

        // OpenGL operations
        void bind(uint32_t slot = 0) const;
        void unbind() const;

        // Getters
        uint32_t getWidth() const { return mWidth; }
        uint32_t getHeight() const { return mHeight; }
        uint32_t getChannels() const { return mChannels; }
        GLuint getID() const { return mRendererID; }
        TextureFormat getFormat() const { return mFormat; }
        TextureFilter getMinFilter() const { return mMinFilter; }
        TextureFilter getMagFilter() const { return mMagFilter; }
        TextureWrap getWrapS() const { return mWrapS; }
        TextureWrap getWrapT() const { return mWrapT; }


        // Setters (should be called before postLoad)
        void setTextureData(unsigned char* data, uint32_t width, uint32_t height, uint32_t channels);
        void setFormat(TextureFormat format) { mFormat = format; }
        void setMinFilter(TextureFilter filter) { mMinFilter = filter; }
        void setMagFilter(TextureFilter filter) { mMagFilter = filter; }
        void setWrapS(TextureWrap wrap) { mWrapS = wrap; }
        void setWrapT(TextureWrap wrap) { mWrapT = wrap; }
        void setGenerateMipmaps(bool generate) { mGenerateMipmaps = generate; }

        pugi::xml_node toNode() const override;

    private:
        void cleanup();
    private:
        // CPU data
        unsigned char* mData;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mChannels;

        // GPU resources
        GLuint mRendererID;

        // Texture parameters
        TextureFormat mFormat;
        TextureFilter mMinFilter;
        TextureFilter mMagFilter;
        TextureWrap mWrapS;
        TextureWrap mWrapT;
        bool mGenerateMipmaps;
    };

}