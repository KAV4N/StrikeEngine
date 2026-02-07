#pragma once
#include "Component.h"
#include "StrikeEngine/Scene/Script.h"
#include "StrikeEngine/Scene/ScriptRegistry.h"

#include <string>
#include <vector>
#include <memory>
#include <pugixml.hpp>

namespace StrikeEngine {
    
    /**
     * @brief Component for attaching and managing scripts on entities.
     * 
     * The LogicComponent allows multiple Script instances to be attached to an entity,
     * providing custom behavior and game logic. Scripts are stored and managed internally
     * as unique pointers.
     * 
     * @note This class is non-copyable and non-movable to ensure safe script management
     */
    class LogicComponent : public Component {
    public:
        /**
         * @brief Default constructor
         */
        LogicComponent() = default;
        
        /**
         * @brief Destructor
         */
        ~LogicComponent() = default;

        LogicComponent(const LogicComponent&) = delete;
        LogicComponent& operator=(const LogicComponent&) = delete;
        LogicComponent(LogicComponent&&) = delete;
        LogicComponent& operator=(LogicComponent&&) = delete;

        /**
         * @brief Get the static type name for this component
         * 
         * @return Constant reference to the static type name "logic"
         */
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "logic";
            return typeName;
        }

        /**
         * @brief Get the type name of this component instance
         * 
         * @return Constant reference to the type name "logic"
         */
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        /**
         * @brief Deserialize logic component data from XML node
         * 
         * @param node The XML node containing script configurations
         */
        void deserialize(const pugi::xml_node& node) override;
        
        /**
         * @brief Add a script of the specified type to this component
         * 
         * @tparam T Script type (must derive from Script)
         * @return Pointer to the newly created script instance
         * @note The component takes ownership of the script
         */
        template<typename T>
        T* addScript();

        /**
         * @brief Remove all scripts of the specified type
         * 
         * @tparam T Script type (must derive from Script)
         * @note This removes all instances of the given script type
         */
        template<typename T>
        void removeScript();

        /**
         * @brief Get the first script of the specified type
         * 
         * @tparam T Script type (must derive from Script)
         * @return Pointer to the script, or nullptr if not found
         */
        template<typename T>
        T* getScript();

        /**
         * @brief Get the first script of the specified type (const)
         * 
         * @tparam T Script type (must derive from Script)
         * @return Const pointer to the script, or nullptr if not found
         */
        template<typename T>
        const T* getScript() const;

        /**
         * @brief Check if a script of the specified type exists
         * 
         * @tparam T Script type (must derive from Script)
         * @return True if the script exists, false otherwise
         */
        template<typename T>
        bool hasScript() const;

        /**
         * @brief Get all scripts attached to this component (const)
         * 
         * @return Constant reference to the vector of script unique pointers
         */
        const std::vector<std::unique_ptr<Script>>& getScripts() const { return mScripts; }
        
        /**
         * @brief Get all scripts attached to this component
         * 
         * @return Reference to the vector of script unique pointers
         */
        std::vector<std::unique_ptr<Script>>& getScripts() { return mScripts; }

        /**
         * @brief Remove all scripts from this component
         */
        void clearScripts();
        
    private:
        /**
         * @brief Add a script by type name (internal use)
         * 
         * @param scriptTypeName Name of the script type to create
         * @return Pointer to the newly created script
         */
       Script* addScript(const std::string& scriptTypeName);
       
    private:
        std::vector<std::unique_ptr<Script>> mScripts; ///< Collection of attached scripts
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
