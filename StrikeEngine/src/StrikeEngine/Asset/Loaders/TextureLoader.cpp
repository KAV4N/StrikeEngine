#include "strikepch.h"
#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Strike {



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
        texture->setState(AssetState::Loading);

        int width, height, channels;
        unsigned char* data = loadImageData(path, width, height, channels);

        if (!data) {
            STRIKE_CORE_ERROR("Failed to load texture image: {}", path.string());
            texture->setState(AssetState::Failed);
            return texture;
        }

        texture->setTextureData(width, height, channels, data);
        freeImageData(data);

        return texture;
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

    std::shared_ptr<Asset> TextureLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        return loadFromNodeInternal<Texture>(node, basePath);
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

    void CubeMapLoader::flipImageHorizontally(unsigned char* data, int width, int height, int channels) {
        int rowSize = width * channels;
        
        for (int y = 0; y < height; y++) {
            unsigned char* row = data + y * rowSize;
            
            for (int x = 0; x < width / 2; x++) {
                int leftIdx = x * channels;
                int rightIdx = (width - 1 - x) * channels;
                
                for (int c = 0; c < channels; c++) {
                    unsigned char tmp = row[leftIdx + c];
                    row[leftIdx + c] = row[rightIdx + c];
                    row[rightIdx + c] = tmp;
                }
            }
        }
    }

    std::filesystem::path CubeMapLoader::findCubemapFace(const std::filesystem::path& directory, const std::string& faceName) {
        const std::array<std::string, 5> extensions = {".jpg", ".jpeg", ".png"};
        
        for (const auto& ext : extensions) {
            std::filesystem::path candidatePath = directory / (faceName + ext);
            if (std::filesystem::exists(candidatePath)) {
                return candidatePath;
            }
        }
        
        return {};
    }

    std::shared_ptr<Asset> CubeMapLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async) {
        auto cubemap = std::make_shared<CubeMap>(id, path);
        cubemap->setState(AssetState::Loading);

        if (!std::filesystem::is_directory(path)) {
            STRIKE_CORE_ERROR("Cubemap path is not a directory: {}", path.string());
            cubemap->setState(AssetState::Failed);
            return cubemap;
        }

        std::array<std::string, 6> faceNames = {
            "right", "left", "top", "bottom", "front", "back"
        };

        unsigned char* faceData[6] = {nullptr};
        int width = 0, height = 0, channels = 0;
        bool loadSuccess = true;

        for (size_t i = 0; i < 6; i++) {
            std::filesystem::path facePath = findCubemapFace(path, faceNames[i]);
            
            if (facePath.empty()) {
                STRIKE_CORE_ERROR("Cubemap face image not found: {} in {}", faceNames[i], path.string());
                loadSuccess = false;
                break;
            }

            int w, h, c;
            unsigned char* data = loadImageData(facePath, w, h, c);

            if (!data) {
                STRIKE_CORE_ERROR("Failed to load cubemap face image: {}", facePath.string());
                loadSuccess = false;
                break;
            }

            if (i == 0) {
                width = w;
                height = h;
                channels = c;
            } else if (w != width || h != height || c != channels) {
                STRIKE_CORE_ERROR("Cubemap face image dimensions mismatch: {}", facePath.string());
                freeImageData(data);
                loadSuccess = false;
                break;
            }

            flipImageHorizontally(data, w, h, c);
            faceData[i] = data;
        }

        if (loadSuccess) {
            cubemap->setCubeMapData(width, height, channels, faceData);
        } else {
            cubemap->setState(AssetState::Failed);
        }

        // Free all loaded image data
        for (int i = 0; i < 6; i++) {
            if (faceData[i]) {
                freeImageData(faceData[i]);
            }
        }

        return cubemap;
    }

    void CubeMapLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderCubemap = std::static_pointer_cast<CubeMap>(placeholder);
        auto loadedCubemap = std::static_pointer_cast<CubeMap>(loaded);

        if (loadedCubemap && placeholderCubemap) {
            int width = loadedCubemap->getWidth();
            int height = loadedCubemap->getHeight();
            int channels = loadedCubemap->getChannels();

            // Create temporary pointers to the loaded face data
            unsigned char* faceData[6];

            for (int i = 0; i < 6; i++) {
                faceData[i] = loadedCubemap->mFaceData[i].data();
            }

            placeholderCubemap->setCubeMapData(width, height, channels, faceData);
        }
    }

    std::shared_ptr<Asset> CubeMapLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        return loadFromNodeInternal<CubeMap>(node, basePath);
    }

}