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

namespace Strike {
    /**
     * @brief Key for identifying instance batches
     * 
     * Used to batch together instances that share the same mesh,
     * texture, and color for efficient rendering.
     */
    struct InstanceKey {
        uintptr_t meshPtr;    /**< Pointer to the mesh */
        uintptr_t texturePtr; /**< Pointer to the texture */
        uint8_t colorR;       /**< Red color component (0-255) */
        uint8_t colorG;       /**< Green color component (0-255) */
        uint8_t colorB;       /**< Blue color component (0-255) */
        
        /**
         * @brief Equality operator for InstanceKey
         * 
         * @param other The other InstanceKey to compare with
         * @return true If all components are equal
         * @return false Otherwise
         */
        bool operator==(const InstanceKey& other) const {
            return meshPtr == other.meshPtr && 
                   texturePtr == other.texturePtr &&
                   colorR == other.colorR &&
                   colorG == other.colorG &&
                   colorB == other.colorB;
        }
    };
    
    /**
     * @brief Hash function for InstanceKey
     */
    struct InstanceKeyHash {
        /**
         * @brief Hash operator for InstanceKey
         * 
         * @param key The InstanceKey to hash
         * @return std::size_t The hash value
         */
        std::size_t operator()(const InstanceKey& key) const {
            std::size_t h1 = std::hash<uintptr_t>{}(key.meshPtr);
            std::size_t h2 = std::hash<uintptr_t>{}(key.texturePtr);
            std::size_t h3 = std::hash<uint32_t>{}((uint32_t(key.colorR) << 16) | 
                                                    (uint32_t(key.colorG) << 8) | 
                                                    uint32_t(key.colorB));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
    
    /**
     * @brief Key for identifying shadow instance batches
     * 
     * Simplified key for shadow rendering that only cares about mesh.
     */
    struct ShadowInstanceKey {
        uintptr_t meshPtr; /**< Pointer to the mesh */
        
        /**
         * @brief Equality operator for ShadowInstanceKey
         * 
         * @param other The other ShadowInstanceKey to compare with
         * @return true If mesh pointers are equal
         * @return false Otherwise
         */
        bool operator==(const ShadowInstanceKey& other) const {
            return meshPtr == other.meshPtr;
        }
    };
    
    /**
     * @brief Hash function for ShadowInstanceKey
     */
    struct ShadowInstanceKeyHash {
        /**
         * @brief Hash operator for ShadowInstanceKey
         * 
         * @param key The ShadowInstanceKey to hash
         * @return std::size_t The hash value
         */
        std::size_t operator()(const ShadowInstanceKey& key) const {
            return std::hash<uintptr_t>{}(key.meshPtr);
        }
    };
    
    /**
     * @brief Batch of instances for rendering
     */
    struct InstanceBatch {
        std::shared_ptr<Mesh> mesh;          /**< The mesh to render */
        std::shared_ptr<Texture> texture;    /**< The texture to apply */
        glm::uvec3 color;                    /**< Color tint (0-255 range) */
        std::vector<glm::mat4> worldMatrices; /**< World transformation matrices for each instance */
        
        /**
         * @brief Clear the batch data
         */
        void clear() {
            worldMatrices.clear();
        }
    };
    
    /**
     * @brief Point light data for rendering
     * 
     * @note Uses alignas(16) for proper GPU memory alignment
     */
    struct alignas(16) PointLight {
        glm::vec4 position;   /**< World position (w=1) */
        glm::vec4 color;      /**< RGB color and intensity */
        float intensity;      /**< Light intensity multiplier */
        float radius;         /**< Light radius for attenuation */
    };
    
    /**
     * @brief Sun/directional light rendering data
     */
    struct SunRenderData {
        Sun* sun; /**< Pointer to the sun light source */
        glm::mat4 lightSpaceMatrix; /**< Light space transformation matrix for shadow mapping */
        
        /**
         * @brief Shadow instance batches for this sun
         */
        std::unordered_map<ShadowInstanceKey, InstanceBatch, ShadowInstanceKeyHash> shadowBatches;
        
        /**
         * @brief Clear the sun render data
         */
        void clear() {
            shadowBatches.clear();
        }
    };
    
    /**
     * @brief Complete camera rendering data
     * 
     * Contains all data needed to render a scene from a specific camera.
     */
    struct CameraRenderData {
        CameraComponent camera;        /**< Camera component with projection/view settings */
        glm::vec3 cameraPosition;      /**< World position of the camera */
        glm::vec3 cameraForward;       /**< Forward direction vector of the camera */
        
        SunRenderData sunData;         /**< Sun/directional light data */
        std::shared_ptr<CubeMap> mSkyboxTexture; /**< Skybox texture for this camera */
        
        /**
         * @brief Instance batches for this camera view
         */
        std::unordered_map<InstanceKey, InstanceBatch, InstanceKeyHash> instanceBatches;
        
        std::vector<PointLight> pointLights; /**< Point lights visible to this camera */
        
        /**
         * @brief Clear all camera render data
         */
        void clear() {
            instanceBatches.clear();
            pointLights.clear();
            sunData.clear();
        }
    };
}