#include "strikepch.h"
#include "AudioListenerComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace Strike {
    REGISTER_COMPONENT(AudioListenerComponent)

    void AudioListenerComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }
    
}