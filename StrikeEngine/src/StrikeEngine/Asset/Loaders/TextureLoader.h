#pragma once
#include "AssetLoader.h"
#include "StrikeEngine/Asset/Types/Texture.h"

namespace StrikeEngine {

    class TextureLoader : public AssetLoader {
    public:
        TextureLoader();
        ~TextureLoader() override = default;

        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) override;
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        unsigned char* loadImageData(const std::filesystem::path& path, int& width, int& height, int& channels);
        void freeImageData(unsigned char* data);
    };

    class CubeMapLoader : public AssetLoader {
    public:
        CubeMapLoader();
        ~CubeMapLoader() override = default;

        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) override;
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        unsigned char* loadImageData(const std::filesystem::path& path, int& width, int& height, int& channels);
        void freeImageData(unsigned char* data);
    };

}