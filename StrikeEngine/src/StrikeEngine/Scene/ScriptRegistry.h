#pragma once
#include "StrikeEngine/Scene/Script.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace Strike {

    /**
     * @brief Registry for script classes to enable dynamic instantiation by class name.
     */
    class ScriptRegistry {
    public:
        /**
         * @brief Type definition for a script factory function.
         * @return Unique pointer to the created Script instance.
         */
        using ScriptFactory = std::function<std::unique_ptr<Script>()>;

        /**
         * @brief Register a script factory function with a class name.
         * @param className The name of the script class.
         * @param factory The factory function to create the script.
         */
        static void registerScriptFactory(const std::string& className, ScriptFactory factory);

        /**
         * @brief Check if a script factory is registered for the given class name.
         * @param className The name of the script class.
         * @return True if a factory is registered, false otherwise.
         */
        static bool hasScriptFactory(const std::string& className);

        /**
         * @brief Check if a script factory is registered for the given type.
         * @tparam T The script type to check.
         * @return True if a factory is registered, false otherwise.
         */
        template<typename T>
        static bool hasScriptFactory() {
            return scriptNameSlot<T>() != nullptr;
        }

        /**
         * @brief Get the registered class name for the given script type.
         * @tparam T The script type to look up.
         * @return The class name string literal registered via REGISTER_SCRIPT.
         */
        template<typename T>
        static const char* getScriptClassName() {
            return scriptNameSlot<T>();
        }

        /**
         * @brief Register a script type with its class name and factory.
         * @tparam T The script type to register.
         * @param className The class name string literal (from REGISTER_SCRIPT macro).
         * @param factory The factory function to create the script.
         */
        template<typename T>
        static void registerScript(const char* className, ScriptFactory factory) {
            registerScriptFactory(className, std::move(factory));
            scriptNameSlot<T>() = className;
        }

        /**
         * @brief Create an instance of a script by class name.
         * @param className The name of the script class.
         * @return Unique pointer to the created script instance, or nullptr if not found.
         */
        static std::unique_ptr<Script> createScript(const std::string& className);

        /**
         * @brief Get a list of all registered script class names.
         * @return Vector of registered script class names.
         */
        static std::vector<std::string> getRegisteredScripts();

    private:
        static std::unordered_map<std::string, ScriptFactory>& getFactories();

        /**
         * @brief Per-type static slot holding a pointer to the registered class name literal.
         * @tparam T The script type.
         * @return Reference to the static name pointer for this type.
         */
        template<typename T>
        static const char*& scriptNameSlot() {
            static const char* slot = nullptr;
            return slot;
        }
    };

}

/**
 * @brief Macro to register a script class with the ScriptRegistry.
 * @param ScriptClass The script class to register.
 */
#define REGISTER_SCRIPT(ScriptClass) \
    namespace {  \
        const bool ScriptClass##_registered = []() { \
            Strike::ScriptRegistry::registerScript<ScriptClass>(#ScriptClass, \
                []() -> std::unique_ptr<Strike::Script> { \
                    return std::make_unique<ScriptClass>(); \
                }); \
            return true; \
        }(); \ 
    }
