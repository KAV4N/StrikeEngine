#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include "Bone.h"

namespace StrikeEngine {
    struct Skeleton {
        std::vector<std::shared_ptr<Bone>> bones;
        std::unordered_map<std::string, std::shared_ptr<Bone>> boneMap;
    };
}