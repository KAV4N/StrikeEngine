#pragma once
#include "StrikeEngine/Asset/Types/Asset.h"
#include <glad/glad.h>
#include <vector>
#include <array>

namespace StrikeEngine {

    class Texture : public Asset {
    public:
        Texture(const std::string& id, const std::filesystem::path& path);
        virtual ~Texture();

        static const std::string& getStaticTypeName();

        const std::string& getTypeName() const override;
        void postLoad() override;

        GLuint getTextureID() const { return mTextureID; }
        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
        int getChannels() const { return mChannels; }

        void bind(unsigned int slot = 0) const;
        void unbind() const;
    private:
        friend class TextureLoader;
        void setTextureData(int width, int height, int channels, unsigned char* data);

    private:
        GLuint mTextureID;
        int mWidth;
        int mHeight;
        int mChannels;
        std::vector<unsigned char> mData;
    };

    class CubeMap : public Asset {
    public:
        CubeMap(const std::string& id, const std::filesystem::path& path);
        virtual ~CubeMap();

        static const std::string& getStaticTypeName();

        const std::string& getTypeName() const override;
        void postLoad() override;

        GLuint getTextureID() const { return mTextureID; }
        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
        int getChannels() const { return mChannels; }

        void bind(unsigned int slot = 0) const;
        void unbind() const;
    private:
        friend class CubeMapLoader;
        void setCubeMapData(int width, int height, int channels, unsigned char* faceData[6]);

    private:
        GLuint mTextureID;
        int mWidth;
        int mHeight;
        int mChannels;
        std::array<std::vector<unsigned char>, 6> mFaceData;
    };

}