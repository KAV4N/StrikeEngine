#include "ScriptComponent.h"
#include "StrikeEngine/Scene/Entity.h"

namespace StrikeEngine {

    ScriptComponent::ScriptComponent() {}

    ScriptComponent::~ScriptComponent() {
        destroyAllScriptInstances();
    }

    void ScriptComponent::addScript(const std::string& scriptType) {
        ScriptInfo info;
        info.scriptType = scriptType;
        info.isActive = true;

        mScriptInfos[scriptType] = info;

        createScriptInstance(scriptType);
    }

    void ScriptComponent::removeScript(const std::string& scriptType) {
        // Destroy instance if it exists
        destroyScriptInstance(scriptType);

        // Remove from info map
        mScriptInfos.erase(scriptType);
    }

    void ScriptComponent::clearScripts() {
        destroyAllScriptInstances();
        mScriptInfos.clear();
    }

    bool ScriptComponent::hasScript(const std::string& scriptType) const {
        return mScriptInfos.find(scriptType) != mScriptInfos.end();
    }

    void ScriptComponent::createScriptInstance(const std::string& scriptType) {
        auto infoIt = mScriptInfos.find(scriptType);
        if (infoIt == mScriptInfos.end()) {
            return; // Script info not found
        }

        // Create the script instance using the registry
        auto script = ScriptRegistry::createScript(scriptType);
        if (script) {
            mScriptInstances[scriptType] = std::move(script);
        }
    }

    void ScriptComponent::destroyScriptInstance(const std::string& scriptType) {
        auto it = mScriptInstances.find(scriptType);
        if (it != mScriptInstances.end()) {
            mScriptInstances.erase(it);
        }
    }

    void ScriptComponent::setScriptActive(const std::string& scriptType, bool active) {
        // Update info
        auto infoIt = mScriptInfos.find(scriptType);
        if (infoIt != mScriptInfos.end()) {
            infoIt->second.isActive = active;
        }

        // Update instance
        auto instanceIt = mScriptInstances.find(scriptType);
        if (instanceIt != mScriptInstances.end() && instanceIt->second) {
            instanceIt->second->setActive(active);
        }
    }

    bool ScriptComponent::isScriptActive(const std::string& scriptType) const {
        auto infoIt = mScriptInfos.find(scriptType);
        if (infoIt != mScriptInfos.end()) {
            return infoIt->second.isActive;
        }
        return false;
    }

    Script* ScriptComponent::getScriptByType(const std::string& scriptType) {
        auto it = mScriptInstances.find(scriptType);
        if (it != mScriptInstances.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const Script* ScriptComponent::getScriptByType(const std::string& scriptType) const {
        auto it = mScriptInstances.find(scriptType);
        if (it != mScriptInstances.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void ScriptComponent::createAllScriptInstances(entt::entity handle, Scene* scene) {
        for (const auto& [scriptType, scriptInfo] : mScriptInfos) {
            createScriptInstance(scriptType);
            auto it = mScriptInstances.find(scriptType);
            if (it != mScriptInstances.end() && it->second) {
                it->second->setEntity(handle, scene);
            }
        }
    }

    void ScriptComponent::destroyAllScriptInstances() {
        mScriptInstances.clear();
    }

    void ScriptComponent::deserialize(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) {
        // Clear existing scripts
        clearScripts();

        // Parse script nodes
        for (pugi::xml_node scriptNode : node.children("script")) {
            std::string scriptType = scriptNode.attribute("type").as_string();

            if (!scriptType.empty() && ScriptRegistry::hasScriptFactory(scriptType)) {
                addScript(scriptType);

                bool active = scriptNode.attribute("active").as_bool(true);
                setScriptActive(scriptType, active);
            }
        }
    }

    void ScriptComponent::serialize(pugi::xml_node& node) const {
        for (const auto& [scriptType, scriptInfo] : mScriptInfos) {
            pugi::xml_node scriptNode = node.append_child("script");
            scriptNode.append_attribute("type") = scriptInfo.scriptType.c_str();

            if (!scriptInfo.isActive) {
                scriptNode.append_attribute("active") = scriptInfo.isActive;
            }
        }
    }
}