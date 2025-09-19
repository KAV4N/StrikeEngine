#pragma once
#include <glad/glad.h>
#include "Asset.h"
#include <string>
#include <filesystem>
#include <pugixml.hpp>

namespace StrikeEngine {

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

    class Texture : public Asset {
    public:
        Texture(const std::string& id, const std::filesystem::path& path, const std::string& name = "");
        virtual ~Texture();

        virtual void bind(uint32_t slot = 0) const = 0;
        virtual void unbind() const = 0;

        GLuint getID() const { return mRendererID; }
        TextureFormat getFormat() const { return mFormat; }
        TextureFilter getMinFilter() const { return mMinFilter; }
        TextureFilter getMagFilter() const { return mMagFilter; }
        TextureWrap getWrapS() const { return mWrapS; }
        TextureWrap getWrapT() const { return mWrapT; }

        void setFormat(TextureFormat format) { mFormat = format; }
        void setMinFilter(TextureFilter filter) { mMinFilter = filter; }
        void setMagFilter(TextureFilter filter) { mMagFilter = filter; }
        void setWrapS(TextureWrap wrap) { mWrapS = wrap; }
        void setWrapT(TextureWrap wrap) { mWrapT = wrap; }
        void setGenerateMipmaps(bool generate) { mGenerateMipmaps = generate; }

    protected:
        void cleanup();

        GLuint mRendererID;
        TextureFormat mFormat;
        TextureFilter mMinFilter;
        TextureFilter mMagFilter;
        TextureWrap mWrapS;
        TextureWrap mWrapT;
        bool mGenerateMipmaps;
    };

    class Texture2D : public Texture {
    public:
        Texture2D(const std::string& id, const std::filesystem::path& path, const std::string& name = "");
        ~Texture2D() override;

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "texture2D";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void postLoad() override;
        void bind(uint32_t slot = 0) const override;
        void unbind() const override;

        uint32_t getWidth() const { return mWidth; }
        uint32_t getHeight() const { return mHeight; }
        uint32_t getChannels() const { return mChannels; }

        void setTextureData(unsigned char* data, uint32_t width, uint32_t height, uint32_t channels);
        pugi::xml_node toNode() const override;

    private:
        unsigned char* mData;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mChannels;
    };

    class CubeMap : public Texture {
    public:
        CubeMap(const std::string& id, const std::filesystem::path& path, const std::string& name = "");
        ~CubeMap() override;

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "cubeMap";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void postLoad() override;
        void bind(uint32_t slot = 0) const override;
        void unbind() const override;

        uint32_t getSize() const { return mSize; }
        uint32_t getChannels() const { return mChannels; }

        void setTextureData(unsigned char* data[6], uint32_t size, uint32_t channels);
        pugi::xml_node toNode() const override;

    private:
        unsigned char* mData[6];
        uint32_t mSize;
        uint32_t mChannels;
    };
}