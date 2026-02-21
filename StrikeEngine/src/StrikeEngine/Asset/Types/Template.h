#pragma once

#include "Asset.h"
#include <pugixml.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Strike {

    class Scene;
    class Entity;

    /**
     * @brief Template asset type.
     * @note Loaded via AssetManager by TemplateLoader.
     */
    class Template : public Asset {
    public:

    /**
     * @brief Construct a new Template object.
     * @param id Unique identifier for the template in AssetManager.
     * @param path File path to the template asset.
     * @note This does not load the template asset; for loading use AssetManager.
     */
        Template(const std::string& id, const std::filesystem::path& path);

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "template";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return Template::getStaticTypeName();
        }


        /**
         * @brief Instantiate the template into the given Entity parent.
         * @param parentEntity The parent entity to which the template entities will be added.
         * @return True if instantiation was successful, false otherwise.
         */
        bool instantiate(Entity parentEntity);

        /**
         * @brief Check if all referenced assets are ready.
         * @return True if all assets are ready, false otherwise.
         * @note This loops through all referenced assets and checks their loading state.
         */
        bool areAssetsReady() const;

    private:
        void setTemplateDoc(const pugi::xml_document& doc);
        void createEntities(Scene& scene, const pugi::xml_node& entitiesNode, Entity parentEntity);
        glm::vec3 parseVector3(const std::string& str);

        friend class TemplateLoader;

    private:
        pugi::xml_document mDoc;
        std::vector<std::string> mReferencedAssets;
    };

}