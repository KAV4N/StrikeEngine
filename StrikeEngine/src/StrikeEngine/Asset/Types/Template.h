#pragma once

#include "Asset.h"
#include <pugixml.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "StrikeEngine/Scene/Scene.h"
#include <unordered_map>

namespace StrikeEngine {

    class Template : public Asset {
    public:
        Template(const std::string& id, const std::filesystem::path& path, const std::string& name = "");

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "template";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }


        void instantiate(Scene& scene, Entity parentEntity, const std::string& idPrefix = "");

    private:
        void createEntities(Scene& scene, const pugi::xml_node& entitiesNode, Entity parentEntity, const std::string& idPrefix);

        glm::vec3 parseVector3(const std::string& str);

        friend class TemplateLoader;

    private:
        pugi::xml_document mDoc;
        std::unordered_map<std::string, std::shared_ptr<Asset>> mAssetReferences;
    };
}