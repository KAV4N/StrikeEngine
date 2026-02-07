#pragma once
#include <string>
#include <unordered_map>
#include <pugixml.hpp>
#include <memory>

namespace StrikeEngine {

    /**
     * @brief Base class for all entity components in the StrikeEngine.
     * 
     * Component provides the fundamental interface that all components must implement.
     * Components can be attached to entities to provide specific functionality.
     * 
     * @note All derived components must implement getTypeName() and deserialize()
     */
    class Component {
    public:
        /**
         * @brief Default constructor
         */
        Component() = default;
        
        /**
         * @brief Virtual destructor
         */
        virtual ~Component() = default;
        
        /**
         * @brief Get the type name of this component
         * 
         * @return Constant reference to the component's type name string
         * @note This must be implemented by all derived classes
         */
        virtual const std::string& getTypeName() const = 0;

        /**
         * @brief Check if the component is active
         * 
         * @return True if the component is active, false otherwise
         */
        bool isActive() const { return mIsActive; }
        
        /**
         * @brief Set the active state of the component
         * 
         * @param active The desired active state
         */
        void setActive(bool active) { mIsActive = active; }

        /**
         * @brief Deserialize component data from XML node
         * 
         * @param node The XML node containing component data
         * @note This must be implemented by all derived classes
         */
        virtual void deserialize(const pugi::xml_node& node) = 0;

    protected:
        bool mIsActive = true; ///< Component active state
    };

}
