#include "TextureLoader.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include <iostream>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace StrikeEngine {

    Texture2DLoader::Texture2DLoader() : AssetLoader(Texture2D::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> Texture2DLoader::load(const std::string& id, const std::filesystem::path& path, bool async) {
        auto texture = std::make_shared<Texture2D>(id, addRootPrefix(path), path.stem().string());

        int width, height, channels;
        unsigned char* data = loadImageData(path, width, height, channels);

        if (!data) {
            throw std::runtime_error("Failed to load texture: " + path.string());
        }

        texture->setTextureData(data, width, height, channels);
        texture->setLoadingState(AssetLoadingState::Loaded);
        if (!async) {
            std::lock_guard<std::mutex> lock(mMutex);
            LoadingTask task;
            task.id = texture->getId();
            task.path = texture->getPath();
            task.placeholderAsset = texture;
            task.flagOnlyPostLoad = true;
            mLoadingTasks.emplace(task.id, std::move(task));
        }

        return texture;
    }

    std::shared_ptr<Asset> Texture2DLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("assetId").as_string();
        std::filesystem::path src = node.attribute("src").as_string();
        bool async = node.attribute("async").as_bool();

        if (assetId.empty() || src.empty()) {
            std::cerr << "Invalid texture2D node: missing assetId or src attribute" << std::endl;
            return nullptr;
        }

        src = resolvePath(src, basePath);

        if (async) {
            return AssetManager::get().loadTextureAsync(assetId, src);
        }
        else {
            return AssetManager::get().loadTexture(assetId, src);
        }
    }

    void Texture2DLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderTexture = std::dynamic_pointer_cast<Texture2D>(placeholder);
        auto loadedTexture = std::dynamic_pointer_cast<Texture2D>(loaded);
        *placeholderTexture = std::move(*loadedTexture);
    }

    std::shared_ptr<Asset> Texture2DLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        return std::make_shared<Texture2D>(id, path, path.stem().string());
    }

    unsigned char* Texture2DLoader::loadImageData(const std::filesystem::path& path, int& width, int& height, int& channels) {
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

        if (!data) {
            std::cerr << "Failed to load image: " << path.string() << " - " << stbi_failure_reason() << std::endl;
            return nullptr;
        }

        size_t dataSize = width * height * channels;
        unsigned char* dataCopy = new unsigned char[dataSize];
        std::memcpy(dataCopy, data, dataSize);
        stbi_image_free(data);
        return dataCopy;
    }

    CubeMapLoader::CubeMapLoader() : AssetLoader(CubeMap::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> CubeMapLoader::load(const std::string& id, const std::filesystem::path& path, bool async) {
        auto cubemap = std::make_shared<CubeMap>(id, addRootPrefix(path), path.stem().string());

        unsigned char* data[6];
        int size, channels;
        if (!loadCubeMapData(path, data, size, channels)) {
            throw std::runtime_error("Failed to load cubemap: " + path.string());
        }

        cubemap->setTextureData(data, size, channels);
        cubemap->setLoadingState(AssetLoadingState::Loaded);
        if (!async) {
            std::lock_guard<std::mutex> lock(mMutex);
            LoadingTask task;
            task.id = cubemap->getId();
            task.path = cubemap->getPath();
            task.placeholderAsset = cubemap;
            task.flagOnlyPostLoad = true;
            mLoadingTasks.emplace(task.id, std::move(task));
        }

        return cubemap;
    }

    std::shared_ptr<Asset> CubeMapLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("assetId").as_string();
        std::filesystem::path src = node.attribute("src").as_string();
        bool async = node.attribute("async").as_bool();

        if (assetId.empty() || src.empty()) {
            std::cerr << "Invalid cubemap node: missing assetId or src attribute" << std::endl;
            return nullptr;
        }

        src = resolvePath(src, basePath);

        if (async) {
            return AssetManager::get().loadCubeMapAsync(assetId, src);
        }
        else {
            return AssetManager::get().loadCubeMap(assetId, src);
        }
    }

    void CubeMapLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderCubeMap = std::dynamic_pointer_cast<CubeMap>(placeholder);
        auto loadedCubeMap = std::dynamic_pointer_cast<CubeMap>(loaded);
        *placeholderCubeMap = std::move(*loadedCubeMap);
    }

    std::shared_ptr<Asset> CubeMapLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        return std::make_shared<CubeMap>(id, path, path.stem().string());
    }

    bool CubeMapLoader::loadCubeMapData(const std::filesystem::path& path, unsigned char* data[6], int& size, int& channels) {
        stbi_set_flip_vertically_on_load(false); // Important for cubemaps
        std::string faces[6] = { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg" };

        int width, height;
        for (int i = 0; i < 6; ++i) {
            std::filesystem::path facePath = path / faces[i];
            data[i] = stbi_load(facePath.string().c_str(), &width, &height, &channels, 0);
            if (!data[i]) {
                std::cerr << "Failed to load cubemap face: " << facePath.string() << " - " << stbi_failure_reason() << std::endl;
                // Clean up already loaded faces
                for (int j = 0; j < i; ++j) {
                    stbi_image_free(data[j]);
                    data[j] = nullptr;
                }
                return false;
            }

            // Validate dimensions
            if (i == 0) {
                size = width;
            }
            else if (width != size || height != size) {
                std::cerr << "Cubemap face " << facePath.string() << " has different dimensions" << std::endl;
                // Clean up all loaded faces
                for (int j = 0; j <= i; ++j) {
                    stbi_image_free(data[j]);
                    data[j] = nullptr;
                }
                return false;
            }
        }

        // Create data copies to match the memory management pattern
        for (int i = 0; i < 6; ++i) {
            size_t dataSize = size * size * channels;
            unsigned char* dataCopy = new unsigned char[dataSize];
            std::memcpy(dataCopy, data[i], dataSize);
            stbi_image_free(data[i]);
            data[i] = dataCopy;
        }

        return true;
    }
}