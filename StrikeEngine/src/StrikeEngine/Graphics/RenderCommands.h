#pragma once
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Scene/Sun.h"
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace StrikeEngine {
    struct InstanceKey {
        uintptr_t meshPtr;
        uintptr_t texturePtr;
        bool operator==(const InstanceKey& other) const {
            return meshPtr == other.meshPtr && texturePtr == other.texturePtr;
        }
    };
    
    struct InstanceKeyHash {
        std::size_t operator()(const InstanceKey& key) const {
            return std::hash<uintptr_t>{}(key.meshPtr) ^
                   (std::hash<uintptr_t>{}(key.texturePtr) << 1);
        }
    };
    
    struct ShadowInstanceKey {
        uintptr_t meshPtr;
        bool operator==(const ShadowInstanceKey& other) const {
            return meshPtr == other.meshPtr;
        }
    };
    
    struct ShadowInstanceKeyHash {
        std::size_t operator()(const ShadowInstanceKey& key) const {
            return std::hash<uintptr_t>{}(key.meshPtr);
        }
    };
    
    struct InstanceBatch {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Texture> texture;
        glm::vec4 color;  // Changed from vec3 to vec4 for blending
        std::vector<glm::mat4> worldMatrices;
        
        void clear() {
            worldMatrices.clear();
        }
    };
    
    struct alignas(16) PointLight {
        glm::vec4 position;
        glm::vec4 color;
        float intensity;
        float radius;
    };
    
    struct SunRenderData {
        Sun* sun;
        glm::mat4 lightSpaceMatrix;
        std::unordered_map<ShadowInstanceKey, InstanceBatch, ShadowInstanceKeyHash> shadowBatches;
        
        void clear() {
            shadowBatches.clear();
        }
    };
    
    struct CameraRenderData {
        CameraComponent camera;
        glm::vec3 cameraPosition;
        
        SunRenderData sunData;
        std::shared_ptr<CubeMap> mSkyboxTexture;
        
        std::unordered_map<InstanceKey, InstanceBatch, InstanceKeyHash> instanceBatches;
        std::vector<PointLight> pointLights;
        
        void clear() {
            instanceBatches.clear();
            pointLights.clear();
            sunData.clear();
        }
    };
}