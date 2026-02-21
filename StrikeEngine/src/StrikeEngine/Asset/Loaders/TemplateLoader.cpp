#include "strikepch.h"

#include "TemplateLoader.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "StrikeEngine/Asset/AssetManager.h"


namespace StrikeEngine {

    TemplateLoader::TemplateLoader() : AssetLoader(Template::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> TemplateLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async) {
        auto templateAsset = std::make_shared<Template>(id, path);
        templateAsset->setState(AssetState::Loading);

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(path.c_str());

        if (!result) {
            STRIKE_CORE_ERROR("Failed to load template XML: {}. Error description: {}", path.string(), result.description());
            templateAsset->setState(AssetState::Failed);
            return templateAsset;
        }

        pugi::xml_node templateNode = doc.child("template");
        if (!templateNode) {
            STRIKE_CORE_ERROR("Invalid template XML: missing <template> root node in {}", path.string());
            templateAsset->setState(AssetState::Failed);
            return templateAsset;
        }

        // Store the document
        templateAsset->setTemplateDoc(doc);

        // Load all assets referenced in the template
        pugi::xml_node assetsNode = templateNode.child("assets");
        if (assetsNode) {
            loadAssets(templateAsset, assetsNode, path.parent_path());
        }

        // WAIT FOR ASYNC ASSET LOADING TO BE READY
        while (!templateAsset->areAssetsReady()) {
            if (!async){
                AssetManager::get().update(); // UPDATE REQUIRED TO INIT LIKE MESH OR MODEL TO BE FULLY READY
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return templateAsset;
    }

    void TemplateLoader::loadAssets(std::shared_ptr<Template> templateAsset,
                                    const pugi::xml_node& assetsNode,
                                    const std::filesystem::path& basePath) 
    {
        auto& assetManager = AssetManager::get();
        for (pugi::xml_node assetNode : assetsNode.children()) {
            std::string assetId = assetNode.attribute("id").as_string();
            std::filesystem::path src = std::filesystem::weakly_canonical(basePath / assetNode.attribute("src").as_string());

            if (assetId == templateAsset->getId()) {
                STRIKE_CORE_WARN("Template '{}': skipping self-referencing asset by id '{}'", templateAsset->getId(), assetId);
                continue;
            }

            if (src == std::filesystem::weakly_canonical(templateAsset->getPath())) {
                STRIKE_CORE_WARN("Template '{}': skipping self-referencing asset by path '{}'", templateAsset->getId(), src.string());
                continue;
            }

            if (assetManager.hasAsset(assetId)) {
                if (assetNode.name() != Template::getStaticTypeName()) {
                    templateAsset->mReferencedAssets.push_back(assetId);
                }
                continue;
            }

            assetManager.deserialize(assetNode, basePath, true);
            if (assetNode.name() != Template::getStaticTypeName()) {
                templateAsset->mReferencedAssets.push_back(assetId);
            }
        }
    }


    void TemplateLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderTemplate = std::dynamic_pointer_cast<Template>(placeholder);
        auto loadedTemplate = std::dynamic_pointer_cast<Template>(loaded);

        if (placeholderTemplate && loadedTemplate) {
            *placeholderTemplate = std::move(*loadedTemplate);
        }
    }

    std::shared_ptr<Asset> TemplateLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        return loadFromNodeInternal<Template>(node, basePath);
    }
}