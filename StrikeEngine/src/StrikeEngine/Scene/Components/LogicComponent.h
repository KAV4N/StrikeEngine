#pragma once
#include "Component.h"
#include "StrikeEngine/Scene/Script.h"
#include "StrikeEngine/Scene/ScriptRegistry.h"

#include <string>
#include <vector>
#include <memory>
#include <pugixml.hpp>

namespace StrikeEngine {
    
    class LogicComponent : public Component {
    public:
        LogicComponent() = default;
        ~LogicComponent() = default;

        LogicComponent(const LogicComponent&) = delete;
        LogicComponent& operator=(const LogicComponent&) = delete;
        LogicComponent(LogicComponent&&) = delete;
        LogicComponent& operator=(LogicComponent&&) = delete;

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "logic";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
        
        template<typename T>
        T* addScript();

        template<typename T>
        void removeScript();

        template<typename T>
        T* getScript();

        template<typename T>
        const T* getScript() const;

        template<typename T>
        bool hasScript() const;

        const std::vector<std::unique_ptr<Script>>& getScripts() const { return mScripts; }
        std::vector<std::unique_ptr<Script>>& getScripts() { return mScripts; }

        void clearScripts();
    private:
       Script* addScript(const std::string& scriptTypeName);
    private:
        std::vector<std::unique_ptr<Script>> mScripts;
    };

    template<typename T>
    T* LogicComponent::addScript() {
        static_assert(std::is_base_of_v<Script, T>, "T must derive from Script");
        
        auto script = std::make_unique<T>();
        T* scriptPtr = script.get();
        mScripts.push_back(std::move(script));
        
        return scriptPtr;
    }

    template<typename T>
    void LogicComponent::removeScript() {
        static_assert(std::is_base_of_v<Script, T>, "T must derive from Script");
        
        mScripts.erase(
            std::remove_if(mScripts.begin(), mScripts.end(),
                [](const std::unique_ptr<Script>& script) {
                    return dynamic_cast<T*>(script.get()) != nullptr;
                }),
            mScripts.end()
        );
    }

    template<typename T>
    T* LogicComponent::getScript() {
        static_assert(std::is_base_of_v<Script, T>, "T must derive from Script");
        
        for (auto& script : mScripts) {
            if (T* castedScript = dynamic_cast<T*>(script.get())) {
                return castedScript;
            }
        }
        return nullptr;
    }

    template<typename T>
    const T* LogicComponent::getScript() const {
        static_assert(std::is_base_of_v<Script, T>, "T must derive from Script");
        
        for (const auto& script : mScripts) {
            if (const T* castedScript = dynamic_cast<const T*>(script.get())) {
                return castedScript;
            }
        }
        return nullptr;
    }

    template<typename T>
    bool LogicComponent::hasScript() const {
        static_assert(std::is_base_of_v<Script, T>, "T must derive from Script");
        return getScript<T>() != nullptr;
    }

}