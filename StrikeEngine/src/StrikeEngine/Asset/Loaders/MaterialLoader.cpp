#include "MaterialLoader.h"
#include "StrikeEngine/Asset/Types/Material.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace StrikeEngine {

    MaterialLoader::MaterialLoader() : AssetLoader(Material::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> MaterialLoader::load(const std::string& id, const std::filesystem::path& path, bool async) {
        

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(path.c_str());

        if (!result) {
            throw std::runtime_error("Failed to load material XML: " + path.string() + " - " + result.description());
        }

        pugi::xml_node materialNode = doc.child("material");
        if (!materialNode) {
            throw std::runtime_error("Invalid material format: no material node found in " + path.string());
        }
        auto material = std::make_shared<Material>(id, path, path.stem().string());
        loadMaterialFromXml(material, materialNode, path.parent_path());


        material->setLoadingState(AssetLoadingState::Loaded);
        if (!async) {
            material->setLoadingState(AssetLoadingState::Ready);
        }

        return material;
    }

    std::shared_ptr<Asset> MaterialLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("assetId").as_string();
        std::filesystem::path src = node.attribute("src").as_string();
        src = basePath / src;

        if (assetId.empty() || src.empty()) {
            std::cerr << "Invalid material node: missing assetId or src attribute" << std::endl;
            return nullptr;
        }

        bool async = node.attribute("async").as_bool();
        if (async)
            return AssetManager::get().loadMaterialAsync(assetId, src);
        else
            return AssetManager::get().loadMaterial(assetId, src);
    }

    std::shared_ptr<Asset> MaterialLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        return std::make_shared<Material>(id, path, path.stem().string());
    }

    void MaterialLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderMaterial = std::dynamic_pointer_cast<Material>(placeholder);
        auto loadedMaterial = std::dynamic_pointer_cast<Material>(loaded);
        placeholderMaterial = std::move(loadedMaterial);
      
    }

    void MaterialLoader::loadMaterialFromXml(std::shared_ptr<Material> material, const pugi::xml_node& materialNode, const std::filesystem::path& basePath) {
        // Load shader
        pugi::xml_node shaderNode = materialNode.child("shader");
        if (shaderNode) {
            std::string shaderAssetId = shaderNode.attribute("assetId").as_string();
            std::filesystem::path vertSrc = shaderNode.attribute("srcVert").as_string();
            std::filesystem::path fragSrc = shaderNode.attribute("srcFrag").as_string();

            if (!shaderAssetId.empty() && !vertSrc.empty() && !fragSrc.empty()) {
                std::filesystem::path vertPath = basePath.parent_path() / vertSrc;
                std::filesystem::path fragPath = basePath.parent_path() / fragSrc;

                auto shader = AssetManager::get().loadShader(shaderAssetId, vertPath, fragPath);
                material->setShader(shader);
            }
        }

        // Load properties
        pugi::xml_node propertiesNode = materialNode.child("properties");
        if (propertiesNode) {
            loadProperties(material, propertiesNode);
        }

        // Load textures
        pugi::xml_node texturesNode = materialNode.child("textures");
        if (texturesNode) {
            loadTextures(material, texturesNode, basePath);
        }
    }

    void MaterialLoader::loadProperties(std::shared_ptr<Material> material, const pugi::xml_node& propertiesNode) {
        for (pugi::xml_node propertyNode : propertiesNode.children("property")) {
            std::string name = propertyNode.attribute("name").as_string();
            std::string type = propertyNode.attribute("type").as_string();
            std::string value = propertyNode.attribute("value").as_string();

            if (name.empty() || type.empty() || value.empty()) {
                continue;
            }

            if (type == "float") {
                material->setFloat(name, std::stof(value));
            }
            else if (type == "int") {
                material->setInt(name, std::stoi(value));
            }
            else if (type == "vec2") {
                // Parse vec2 format: "x,y"
                std::istringstream ss(value);
                std::string token;
                glm::vec2 vec;
                int i = 0;
                while (std::getline(ss, token, ',') && i < 2) {
                    vec[i++] = std::stof(token);
                }
                material->setVec2(name, vec);
            }
            else if (type == "vec3") {
                material->setVec3(name, parseVec3(value));
            }
            else if (type == "vec4") {
                material->setVec4(name, parseVec4(value));
            }
        }
    }

    void MaterialLoader::loadTextures(std::shared_ptr<Material> material, const pugi::xml_node& texturesNode, const std::filesystem::path& basePath) {
        uint32_t slot = 0;

        for (pugi::xml_node textureNode : texturesNode.children("texture2D")) {
            std::string name = textureNode.attribute("name").as_string();
            std::string src = textureNode.attribute("src").as_string();

            if (name.empty() || src.empty()) {
                continue;
            }

            std::filesystem::path texturePath = basePath.parent_path() / src;
          
            auto texture = AssetManager::get().loadTexture(src, texturePath);
            if (texture) {
                material->addTexture(slot, texture);

                material->setInt(name, static_cast<int>(slot));
            }

            slot++;
        }
    }

    glm::vec3 MaterialLoader::parseVec3(const std::string& value) {
        std::istringstream ss(value);
        std::string token;
        glm::vec3 vec(0.0f);
        int i = 0;
        while (std::getline(ss, token, ',') && i < 3) {
            vec[i++] = std::stof(token);
        }
        return vec;
    }

    glm::vec4 MaterialLoader::parseVec4(const std::string& value) {
        std::istringstream ss(value);
        std::string token;
        glm::vec4 vec(0.0f);
        int i = 0;
        while (std::getline(ss, token, ',') && i < 4) {
            vec[i++] = std::stof(token);
        }
        return vec;
    }

}