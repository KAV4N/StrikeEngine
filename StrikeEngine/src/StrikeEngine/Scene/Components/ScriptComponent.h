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
        std::string scriptType; // Now stores the class name instead of path
        bool isActive = true;
    };

    class ScriptComponent : public Component {
    public:
        ScriptComponent();
        ~ScriptComponent();

        // Delete copy constructor and assignment operator
        ScriptComponent(const ScriptComponent&) = delete;
        ScriptComponent& operator=(const ScriptComponent&) = delete;

        // Add move constructor and assignment operator
        ScriptComponent(ScriptComponent&&) = default;
        ScriptComponent& operator=(ScriptComponent&&) = default;

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
        void deserialize(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Script management
        void addScript(const std::string& scriptType);
        void removeScript(const std::string& scriptType);
        void clearScripts();
        bool hasScript(const std::string& scriptType) const;

        // Script instance management
        void createScriptInstance(const std::string& scriptType);
        void destroyScriptInstance(const std::string& scriptType);
        void setScriptActive(const std::string& scriptType, bool active);
        bool isScriptActive(const std::string& scriptType) const;

        // Script access
        template<typename T>
        T* getScript(const std::string& scriptType);

        template<typename T>
        const T* getScript(const std::string& scriptType) const;

        Script* getScriptByType(const std::string& scriptType);
        const Script* getScriptByType(const std::string& scriptType) const;

        // Get all scripts
        const std::unordered_map<std::string, ScriptInfo>& getScriptInfos() const { return mScriptInfos; }
        std::unordered_map<std::string, std::unique_ptr<Script>>& getScriptInstances() { return mScriptInstances; }
        const std::unordered_map<std::string, std::unique_ptr<Script>>& getScriptInstances() const { return mScriptInstances; }

        void createAllScriptInstances(entt::entity handle, Scene* scene);

    private:
        std::unordered_map<std::string, ScriptInfo> mScriptInfos;
        std::unordered_map<std::string, std::unique_ptr<Script>> mScriptInstances;

        void destroyAllScriptInstances();
    };

    // Template implementations
    template<typename T>
    T* ScriptComponent::getScript(const std::string& scriptType) {
        auto it = mScriptInstances.find(scriptType);
        if (it != mScriptInstances.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    const T* ScriptComponent::getScript(const std::string& scriptType) const {
        auto it = mScriptInstances.find(scriptType);
        if (it != mScriptInstances.end()) {
            return dynamic_cast<const T*>(it->second.get());
        }
        return nullptr;
    }
}