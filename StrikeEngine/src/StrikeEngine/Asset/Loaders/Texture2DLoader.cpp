#include "Texture2DLoader.h"
#include "StrikeEngine/Asset/Types/Texture2D.h"
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
        std::string srcString = src.string();

        bool async = node.attribute("async").as_bool();

        if (assetId.empty() || src.empty()) {
            std::cerr << "Invalid texture2D node: missing assetId or src attribute" << std::endl;
            return nullptr;
        }

        src = resolvePath(src, basePath);

        // Check for optional texture parameters
        std::string minFilter = node.attribute("minFilter").as_string("linear");
        std::string magFilter = node.attribute("magFilter").as_string("linear");
        std::string wrapS = node.attribute("wrapS").as_string("repeat");
        std::string wrapT = node.attribute("wrapT").as_string("repeat");
        bool generateMipmaps = node.attribute("generateMipmaps").as_bool(true);
        std::shared_ptr<Texture2D> texture;

        if (async) {
            texture = std::dynamic_pointer_cast<Texture2D>(AssetManager::get().loadTextureAsync(assetId, src));
        }
        else {
            texture = std::dynamic_pointer_cast<Texture2D>(AssetManager::get().loadTexture(assetId, src));
        }

        if (texture) {
            // Set texture parameters
            if (minFilter == "nearest") texture->setMinFilter(TextureFilter::Nearest);
            else if (minFilter == "linear_mipmap_linear") texture->setMinFilter(TextureFilter::LinearMipmapLinear);
            else if (minFilter == "linear_mipmap_nearest") texture->setMinFilter(TextureFilter::LinearMipmapNearest);
            else if (minFilter == "nearest_mipmap_linear") texture->setMinFilter(TextureFilter::NearestMipmapLinear);
            else if (minFilter == "nearest_mipmap_nearest") texture->setMinFilter(TextureFilter::NearestMipmapNearest);

            if (magFilter == "nearest") texture->setMagFilter(TextureFilter::Nearest);

            if (wrapS == "clamp_to_edge") texture->setWrapS(TextureWrap::ClampToEdge);
            else if (wrapS == "clamp_to_border") texture->setWrapS(TextureWrap::ClampToBorder);
            else if (wrapS == "mirrored_repeat") texture->setWrapS(TextureWrap::MirroredRepeat);

            if (wrapT == "clamp_to_edge") texture->setWrapT(TextureWrap::ClampToEdge);
            else if (wrapT == "clamp_to_border") texture->setWrapT(TextureWrap::ClampToBorder);
            else if (wrapT == "mirrored_repeat") texture->setWrapT(TextureWrap::MirroredRepeat);

            texture->setGenerateMipmaps(generateMipmaps);
        }

        return texture;
    }

    void Texture2DLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderTexture = std::dynamic_pointer_cast<Texture2D>(placeholder);
        auto loadedTexture = std::dynamic_pointer_cast<Texture2D>(loaded);

        *placeholderTexture = std::move(*loadedTexture);
        //loadedTexture->setTextureData({}, 0, 0, 0);
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

        // Create a copy to manage memory properly
        size_t dataSize = width * height * channels;
        unsigned char* dataCopy = new unsigned char[dataSize];
        std::memcpy(dataCopy, data, dataSize);

        stbi_image_free(data);

        return dataCopy;
    }

}