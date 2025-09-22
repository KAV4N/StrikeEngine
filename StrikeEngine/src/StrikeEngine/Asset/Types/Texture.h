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

    class Texture : public Asset {
    public:
        Texture(const std::string& id, const std::filesystem::path& path, const std::string& name = "");
        virtual ~Texture();

        virtual void bind(uint32_t slot = 0) const = 0;
        virtual void unbind() const = 0;

        GLuint getID() const { return mRendererID; }
        TextureFormat getFormat() const { return mFormat; }

        void setFormat(TextureFormat format) { mFormat = format; }

    protected:
        void cleanup();

        GLuint mRendererID;
        TextureFormat mFormat;
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
