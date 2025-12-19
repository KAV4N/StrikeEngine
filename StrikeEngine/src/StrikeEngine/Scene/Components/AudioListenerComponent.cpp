#include "strikepch.h"
#include "AudioListenerComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {
    REGISTER_COMPONENT(AudioListenerComponent)

    void AudioListenerComponent::deserialize(const pugi::xml_node& node) {

    }

    void AudioListenerComponent::serialize(pugi::xml_node& node) const {
        std::cout<<"test";
    }
    
}