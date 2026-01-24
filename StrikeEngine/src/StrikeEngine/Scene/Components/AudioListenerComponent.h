#pragma once
#include "Component.h"
#include <pugixml.hpp>

namespace StrikeEngine {

    class AudioListenerComponent : public Component {
    public:
        AudioListenerComponent() = default;

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "audiolistener";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
    };
}