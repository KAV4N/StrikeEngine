#include "strikepch.h"
#include "TextureLoader.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace StrikeEngine {

    // ============ TextureLoader ============

    TextureLoader::TextureLoader() : AssetLoader("Texture") {}

    unsigned char* TextureLoader::loadImageData(const std::filesystem::path& path, int& width, int& height, int& channels) {
        stbi_set_flip_vertically_on_load(true);
        return stbi_load(path.string().c_str(), &width, &height, &channels, 0);
    }

    void TextureLoader::freeImageData(unsigned char* data) {
        if (data) {
            stbi_image_free(data);
        }
    }

    std::shared_ptr<Asset> TextureLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async) {
        auto texture = std::make_shared<Texture>(id, path);
        texture->setLoadingState(AssetLoadingState::Loading);

        int width, height, channels;
        unsigned char* data = loadImageData(path, width, height, channels);

        if (!data) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            texture->setLoadingState(AssetLoadingState::FAILED);
            return texture;
        }

        texture->setTextureData(width, height, channels, data);
        freeImageData(data);

        return texture;
    }

    std::shared_ptr<Asset> TextureLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string id = node.attribute("id").as_string();
        std::string srcStr = node.attribute("src").as_string();
        
        if (id.empty() || srcStr.empty()) {
            std::cerr << "Invalid texture node: missing id or src" << std::endl;
            return nullptr;
        }

        std::filesystem::path path = resolvePath(srcStr, basePath);
        return load(id, path);
    }

    std::shared_ptr<Asset> TextureLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        auto placeholder = std::make_shared<Texture>(id, path);
        placeholder->setLoadingState(AssetLoadingState::Loading);
        return placeholder;
    }

    void TextureLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderTexture = std::static_pointer_cast<Texture>(placeholder);
        auto loadedTexture = std::static_pointer_cast<Texture>(loaded);

        if (loadedTexture && placeholderTexture) {
            int width = loadedTexture->getWidth();
            int height = loadedTexture->getHeight();
            int channels = loadedTexture->getChannels();
            
            std::vector<unsigned char> tempData(static_cast<size_t>(width) * height * channels);
            placeholderTexture->setTextureData(width, height, channels, tempData.data());
        }
    }

    // ============ CubeMapLoader ============

    CubeMapLoader::CubeMapLoader() : AssetLoader("CubeMap") {}

    unsigned char* CubeMapLoader::loadImageData(const std::filesystem::path& path, int& width, int& height, int& channels) {
        stbi_set_flip_vertically_on_load(false);
        return stbi_load(path.string().c_str(), &width, &height, &channels, 0);
    }

    void CubeMapLoader::freeImageData(unsigned char* data) {
        if (data) {
            stbi_image_free(data);
        }
    }

    std::shared_ptr<Asset> CubeMapLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async) {
        auto cubemap = std::make_shared<CubeMap>(id, path);
        cubemap->setLoadingState(AssetLoadingState::Loading);

        std::array<std::string, 6> faceNames = {
            "right", "left", "top", "bottom", "front", "back"
        };

        std::array<std::vector<unsigned char>, 6> faceData;
        int width = 0, height = 0, channels = 0;
        bool loadSuccess = true;

        std::filesystem::path parentPath = path.parent_path();
        
        std::string baseName = path.stem().string();
        std::string extension = path.extension().string();

        for (size_t i = 0; i < 6; i++) {
            std::filesystem::path facePath = parentPath / (baseName + "_" + faceNames[i] + extension);
            
            int w, h, c;
            unsigned char* data = loadImageData(facePath, w, h, c);

            if (!data) {
                std::cerr << "Failed to load cubemap face: " << facePath << std::endl;
                loadSuccess = false;
                break;
            }

            if (i == 0) {
                width = w;
                height = h;
                channels = c;
            } else if (w != width || h != height || c != channels) {
                std::cerr << "Cubemap face dimensions mismatch: " << facePath << std::endl;
                freeImageData(data);
                loadSuccess = false;
                break;
            }

            size_t dataSize = static_cast<size_t>(w) * h * c;
            faceData[i].resize(dataSize);
            std::memcpy(faceData[i].data(), data, dataSize);
            freeImageData(data);
        }

        if (!loadSuccess) {
            cubemap->setLoadingState(AssetLoadingState::FAILED);
            return cubemap;
        }

        cubemap->setCubeMapData(width, height, channels, faceData);
        return cubemap;
    }

    std::shared_ptr<Asset> CubeMapLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string id = node.attribute("id").as_string();
        std::string srcStr = node.attribute("src").as_string();
        
        if (id.empty() || srcStr.empty()) {
            std::cerr << "Invalid cubemap node: missing id or src" << std::endl;
            return nullptr;
        }

        std::filesystem::path path = resolvePath(srcStr, basePath);
        return load(id, path);
    }

    std::shared_ptr<Asset> CubeMapLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        auto placeholder = std::make_shared<CubeMap>(id, path);
        placeholder->setLoadingState(AssetLoadingState::Loading);
        return placeholder;
    }

    void CubeMapLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderCubemap = std::static_pointer_cast<CubeMap>(placeholder);
        auto loadedCubemap = std::static_pointer_cast<CubeMap>(loaded);

        if (loadedCubemap && placeholderCubemap) {
            int width = loadedCubemap->getWidth();
            int height = loadedCubemap->getHeight();
            
            std::array<std::vector<unsigned char>, 6> emptyFaces;
            placeholderCubemap->setCubeMapData(width, height, 0, emptyFaces);
        }
    }

}