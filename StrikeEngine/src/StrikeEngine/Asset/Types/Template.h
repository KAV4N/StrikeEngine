#pragma once

#include "Asset.h"
#include <pugixml.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace StrikeEngine {

    class Scene;
    class Entity;

    class Template : public Asset {
    public:
        Template(const std::string& id, const std::filesystem::path& path);

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "template";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        bool instantiate(Entity parentEntity);

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