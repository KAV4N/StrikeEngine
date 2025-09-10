#pragma once

#include <pugixml.hpp>
#include <filesystem>
#include <memory>
#include <string>

#include "StrikeEngine/Asset/Loaders/AssetLoader.h"
#include "StrikeEngine/Asset/Types/Shader.h"

namespace StrikeEngine {

    class ShaderLoader : public AssetLoader {
    public:
        ShaderLoader();

        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

        // Specific shader loading method
        std::shared_ptr<Shader> loadShader(const std::string& id, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, bool async=false);
        std::shared_ptr<Shader> loadShaderAsync(const std::string& id, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, std::shared_ptr<Asset> placeholderAsset);

    protected:
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;

    private:
        std::string readFile(const std::filesystem::path& path);
    };

}