#pragma once

#include "Component.h"


#include <vector>
#include <entt/entt.hpp>
#include <pugixml.hpp>


namespace StrikeEngine {

    class Entity;
    class Scene; 

    class HierarchyComponent : public Component {
    public:
        HierarchyComponent() = default;
        explicit HierarchyComponent(entt::entity parent);

        HierarchyComponent(const HierarchyComponent&) = delete;
        HierarchyComponent& operator=(const HierarchyComponent&) = delete;
        HierarchyComponent(HierarchyComponent&&) = delete;
        HierarchyComponent& operator=(HierarchyComponent&&) = delete;

        static const std::string& getStaticTypeName();
        const std::string& getTypeName() const override;

        void setParent(Entity parent);
        void addChild(Entity child);
        void destroyChildren(); // deletes all entities in children from game world

        // Children access
        std::vector<Entity> getChildren() const;
        size_t getChildCount() const;
        bool hasChildren() const;

        // Parent access
        Entity getParent() const;
        bool isRoot() const; // Parent == null

        // Internal accessors 
        entt::entity getParentHandle() const { return mParent; }
        const std::vector<entt::entity>& getChildrenHandles() const { return mChildren; }

        // Serialization
        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;


    private:
        friend class Scene;

        entt::entity mEntity; 
        Scene* mScene;

        entt::entity mParent = entt::null;
        std::vector<entt::entity> mChildren;
    };

} 