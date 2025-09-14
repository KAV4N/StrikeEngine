#include "TemplateLoader.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Asset/ModelParser.h"
#include <stdexcept>


namespace StrikeEngine {

    TemplateLoader::TemplateLoader() : AssetLoader(Template::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> TemplateLoader::load(const std::string& id, const std::filesystem::path& path, bool async) {
        auto templateAsset = std::make_shared<Template>(id, path, path.stem().string());

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(path.c_str());

        if (!result) {
            throw std::runtime_error("Failed to load template XML: " + path.string());
        }

        pugi::xml_node templateNode = doc.child("template");
        if (!templateNode) {
            throw std::runtime_error("Invalid template format: no template node found in " + path.string());
        }

        std::string templateName = templateNode.attribute("name").as_string();
        templateAsset->setName(templateName);

        AssetManager& assetManager = AssetManager::get();
        pugi::xml_node assetsNode = templateNode.child("assets");

        if (assetsNode) {
            AssetManager::get().deserialize(assetsNode, templateAsset->mAssetReferences, path.parent_path());
        }

        templateAsset->setTemplateDoc(doc);
        templateAsset->setLoadingState(AssetLoadingState::Loaded);
        if (!async) {
            templateAsset->setLoadingState(AssetLoadingState::Ready);
        }
        return templateAsset;
    }

    std::shared_ptr<Asset> TemplateLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("assetId").as_string();
        std::filesystem::path src = node.attribute("src").as_string();
        std::filesystem::path srcModel = node.attribute("srcModel").as_string();

        src = basePath / src;
        srcModel = basePath / srcModel;

        if (assetId.empty() || src.empty()) {
            std::cerr << "Invalid mesh node: missing assetId or srcModel attribute" << std::endl;
            return nullptr;
        }
        if (!std::filesystem::exists(src)) {
            if (srcModel.empty() || !std::filesystem::exists(srcModel)) {
                throw std::runtime_error("Failed to load object:");
            }
            ModelParser parser;
            parser.parseModel(srcModel, src);
        }

        bool async = node.attribute("async").as_bool();
        if (async)
            return AssetManager::get().loadTemplateAsync(assetId, src);
        else
            return AssetManager::get().loadTemplate(assetId, src);
    }

    void TemplateLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded)
    {
        auto placeholderAsset = std::dynamic_pointer_cast<Template>(placeholder);
        auto loadedAsset = std::dynamic_pointer_cast<Template>(loaded);
        placeholderAsset = std::move(loadedAsset);
    }


    std::shared_ptr<Asset> TemplateLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        return std::make_shared<Template>(id, path, path.stem().string());
    }

}