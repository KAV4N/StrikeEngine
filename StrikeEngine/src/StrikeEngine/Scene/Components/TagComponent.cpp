#include "TagComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {

    void TagComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("tag")) {
            mTag = attr.as_string();
        }
    }

    void TagComponent::serialize(pugi::xml_node& node) const {
        if (!mTag.empty()) {
            node.append_attribute("tag") = mTag.c_str();
        }
    }

}    