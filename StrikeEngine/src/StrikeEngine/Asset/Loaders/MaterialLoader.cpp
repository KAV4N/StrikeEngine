#include "strikepch.h"
#include "MaterialLoader.h"

#include "StrikeEngine/Asset/Loaders/AssetLoader.h"
#include "StrikeEngine/Asset/Types/Material.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {

    MaterialLoader::MaterialLoader() : AssetLoader(Material::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> MaterialLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(path.c_str());

        if (!result) {
            throw std::runtime_error("Failed to load material XML: " + path.string() + " - " + result.description());
        }

        pugi::xml_node materialNode = doc.child("material");
        if (!materialNode) {
            throw std::runtime_error("Invalid material format: no material node found in " + path.string());
        }

        auto material = std::make_shared<Material>(id, addRootPrefix(path));
        loadMaterialFromXml(material, materialNode, path.parent_path());

        return material;
    }

    std::shared_ptr<Asset> MaterialLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("id").as_string();
        std::filesystem::path src = node.attribute("src").as_string();

        src = resolvePath(src, basePath);

        if (assetId.empty() || src.empty()) {
            throw std::runtime_error("Invalid material node: missing assetId or src attribute");
        }

        bool async = node.attribute("async").as_bool();
        if (async)
            return AssetManager::get().loadMaterialAsync(assetId, src);
        else
            return AssetManager::get().loadMaterial(assetId, src);
    }

    std::shared_ptr<Asset> MaterialLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        return std::make_shared<Material>(id, path);
    }

    void MaterialLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderMaterial = std::dynamic_pointer_cast<Material>(placeholder);
        auto loadedMaterial = std::dynamic_pointer_cast<Material>(loaded);
        *placeholderMaterial = std::move(*loadedMaterial);
    }

    void MaterialLoader::loadMaterialFromXml(std::shared_ptr<Material> material, const pugi::xml_node& materialNode, const std::filesystem::path& basePath) {
        // Load PBR properties
        loadProperties(material, materialNode);

        // Load textures
        loadTextures(material, materialNode, basePath);
    }

    void MaterialLoader::loadProperties(std::shared_ptr<Material> material, const pugi::xml_node& materialNode) {
        // Load baseColor from baseColor node
        if (auto baseColorNode = materialNode.child("baseColor")) {
            material->setBaseColor(parseVec3(baseColorNode));
        }

        // Load metallic from metallic node
        if (auto metallicNode = materialNode.child("metallic")) {
            material->setMetallic(metallicNode.attribute("value").as_float());
        }

        // Load roughness from roughness node
        if (auto roughnessNode = materialNode.child("roughness")) {
            material->setRoughness(roughnessNode.attribute("value").as_float());
        }
    }

    void MaterialLoader::loadTextures(std::shared_ptr<Material> material, const pugi::xml_node& materialNode, const std::filesystem::path& basePath) {
        // Load base color texture from baseColor node
        if (auto baseColorNode = materialNode.child("baseColor")) {
            if (auto textureNode = baseColorNode.child("texture")) {
                std::filesystem::path src = textureNode.attribute("src").as_string();
                std::string id = textureNode.attribute("id").as_string();
                src = resolvePath(src, basePath);
                auto texture = AssetManager::get().loadTexture(id, src);
                if (texture) {
                    material->setBaseColorTexture(std::dynamic_pointer_cast<Texture>(texture));
                }
            }
        }

        // Load normal texture from normal node
        if (auto normalNode = materialNode.child("normal")) {
            if (auto textureNode = normalNode.child("texture")) {
                std::filesystem::path src = textureNode.attribute("src").as_string();
                std::string id = textureNode.attribute("id").as_string();
                src = resolvePath(src, basePath);
                auto texture = AssetManager::get().loadTexture(id, src);
                if (texture) {
                    material->setNormalTexture(std::dynamic_pointer_cast<Texture>(texture));
                }
            }
        }

        if (auto metallicNode = materialNode.child("metallic")) {
            if (auto textureNode = metallicNode.child("texture")) {
                std::filesystem::path src = textureNode.attribute("src").as_string();
                std::string id = textureNode.attribute("id").as_string();
                src = resolvePath(src, basePath);
                auto texture = AssetManager::get().loadTexture(id, src);
                if (texture) {
                    material->setMetallicTexture(std::dynamic_pointer_cast<Texture>(texture));
                }
            }
        }

        if (auto roughnessNode = materialNode.child("roughness")) {
            if (auto textureNode = roughnessNode.child("texture")) {
                std::filesystem::path src = textureNode.attribute("src").as_string();
                std::string id = textureNode.attribute("id").as_string();
                src = resolvePath(src, basePath);
                auto texture = AssetManager::get().loadTexture(id, src);
                if (texture) {
                    material->setRoughnessTexture(std::dynamic_pointer_cast<Texture>(texture));
                }
            }
        }
    }

    glm::ivec3 MaterialLoader::parseVec3(const pugi::xml_node& node) {
        glm::ivec3 vec(0);
        vec.r = node.attribute("r").as_int();
        vec.g = node.attribute("g").as_int();
        vec.b = node.attribute("b").as_int();
        return vec;
    }
}