#pragma once
#include "StrikeEngine/Asset/Types/Asset.h"
#include <glad/glad.h>
#include <vector>
#include <array>

namespace StrikeEngine {

    /**
     * @brief Texture asset type.
     * @note Loaded via AssetManager by TextureLoader.
     */
    class Texture : public Asset {
    public:
    /**
     * @brief Construct a new Texture object.
     * @param id Unique identifier for the texture in AssetManager.
     * @param path File path to the texture asset.
     */
        Texture(const std::string& id, const std::filesystem::path& path);
        ~Texture();

        static const std::string& getStaticTypeName(){
            static const std::string typeName = "texture";
            return typeName;
        }

        const std::string& getTypeName() const override{
            return Texture::getStaticTypeName();
        }

        /**
         * @brief Get the OpenGL texture ID.
         * @return OpenGL texture ID.
         */
        GLuint getTextureID() const { return mTextureID; }

        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
        int getChannels() const { return mChannels; }

    protected:
        void postLoad() override;
    private:
        friend class TextureLoader;
        friend class GeometryRenderPass;

        void setTextureData(int width, int height, int channels, unsigned char* data);
        
        void bind(unsigned int slot = 0) const;
        void unbind() const;

    private:
        GLuint mTextureID;
        int mWidth;
        int mHeight;
        int mChannels;
        std::vector<unsigned char> mData;
    };

    /**
     * @brief CubeMap asset type.
     * @note Loaded via AssetManager by CubeMapLoader.
     */
    class CubeMap : public Asset {
    public:
        /**
         * @brief Construct a new CubeMap object.
         * @param id Unique identifier for the cubemap in AssetManager.
         * @param path File path to the cubemap asset.
         */
        CubeMap(const std::string& id, const std::filesystem::path& path);
        ~CubeMap();

        static const std::string& getStaticTypeName(){
            static const std::string typeName = "cubeMap";
            return typeName;
        }

        const std::string& getTypeName() const override{
            return CubeMap::getStaticTypeName();
        }



        GLuint getTextureID() const { return mTextureID; }
        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
        int getChannels() const { return mChannels; }

    protected:


        /**
         * @brief Called after the asset has been loaded to perform any additional setup.
         */
        void postLoad() override;
    private:
        friend class CubeMapLoader;
        friend class SkyboxRenderPass;
        void setCubeMapData(int width, int height, int channels, unsigned char* faceData[6]);
        void bind(unsigned int slot = 0) const;
        void unbind() const;

    private:
        GLuint mTextureID;
        int mWidth;
        int mHeight;
        int mChannels;
        std::array<std::vector<unsigned char>, 6> mFaceData;
    };

}