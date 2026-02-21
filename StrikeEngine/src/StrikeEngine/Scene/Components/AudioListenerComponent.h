#pragma once
#include "Component.h"
#include <pugixml.hpp>

namespace Strike {

    /**
     * @brief Component that acts as an audio listener in 3D space.
     * 
     * The AudioListenerComponent represents the position and orientation from which
     * the player/camera hears 3D audio. Only one active audio listener should exist
     * in the scene at a time.
     * 
     * @note Typically attached to the main camera entity
     */
    class AudioListenerComponent : public Component {
    public:
        /**
         * @brief Default constructor
         */
        AudioListenerComponent() = default;

        /**
         * @brief Get the static type name for this component
         * 
         * @return Constant reference to the static type name "audiolistener"
         */
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "audiolistener";
            return typeName;
        }

        /**
         * @brief Get the type name of this component instance
         * 
         * @return Constant reference to the type name "audiolistener"
         */
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        /**
         * @brief Deserialize audio listener data from XML node
         * 
         * @param node The XML node containing audio listener configuration
         */
        void deserialize(const pugi::xml_node& node) override;
    };
}
