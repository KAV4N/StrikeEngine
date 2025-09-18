#pragma once
#include "Component.h"
#include "StrikeEngine/Scene/Script.h"
#include "StrikeEngine/Scene/ScriptRegistry.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <pugixml.hpp>

namespace StrikeEngine {
    class Entity;
    class Scene;

    struct ScriptInfo {
        std::string scriptType; 
        bool isActive = true;
    };

    class ScriptComponent : public Component {
    public:
        ScriptComponent();
        ~ScriptComponent();

        // Delete copy constructor and assignment operator
        ScriptComponent(const ScriptComponent&) = delete;
        ScriptComponent& operator=(const ScriptComponent&) = delete;

        // Static type name for registration
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "scriptComponent";
            return typeName;
        }

        // Virtual method implementation
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        // Serialization methods
        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Script management
        void addScript(const std::string& scriptType);
        void destroyScript(const std::string& scriptType);
        void clearScripts();
        bool hasScript(const std::string& scriptType) const;
        void setScriptActive(const std::string& scriptType, bool active);

        // Script access
        Script* getScriptByType(const std::string& scriptType);
        const Script* getScriptByType(const std::string& scriptType) const;

        // Get all scripts
        const std::unordered_map<std::string, ScriptInfo>& getScriptInfos() const { return mScriptInfos; }
        std::unordered_map<std::string, std::unique_ptr<Script>>& getScriptInstances() { return mScriptInstances; }
        const std::unordered_map<std::string, std::unique_ptr<Script>>& getScriptInstances() const { return mScriptInstances; }

        bool isValid() { return mEntity; }
    private:
        friend class ScriptSystem;
        // Script instance management
        void createScriptInstance(const std::string& scriptType);
        void destroyScriptInstance(const std::string& scriptType);
        bool isScriptActive(const std::string& scriptType) const;
        void destroyAllScriptInstances();
        void asignEntityToScripts(const Entity& entity);
    private:
        std::unordered_map<std::string, ScriptInfo> mScriptInfos;
        std::unordered_map<std::string, std::unique_ptr<Script>> mScriptInstances;

        Entity mEntity;

    };
}