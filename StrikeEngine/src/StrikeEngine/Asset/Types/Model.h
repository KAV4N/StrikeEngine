#pragma once
#include "Asset.h"
#include "glad/glad.h"
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <pugixml.hpp>

namespace StrikeEngine {

    class GraphicsAsset;

    struct Bounds {
        glm::vec3 aabbMax, aabbMin;
        Bounds() {
            float minNum = std::numeric_limits<float>::lowest();
            float maxNum = std::numeric_limits<float>::max();
            aabbMin = glm::vec3(maxNum, maxNum, maxNum);
            aabbMax = glm::vec3(minNum, minNum, minNum);
        }
        Bounds(const glm::vec3& p) {
            aabbMax = p;
            aabbMin = p;
        }
        Bounds(const glm::vec3& min, const glm::vec3& max) {
            aabbMax = max;
            aabbMin = min;
        }
        Bounds(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
            aabbMax = glm::max(glm::max(a, b), c);
            aabbMin = glm::min(glm::min(a, b), c);
        }
        void unionBounds(const Bounds& bounds) {
            aabbMax = glm::max(aabbMax, bounds.aabbMax);
            aabbMin = glm::min(aabbMin, bounds.aabbMin);
        }
        float area() const {
            glm::vec3 e = aabbMax - aabbMin;
            return e.x * e.y + e.y * e.z + e.z * e.x;
        }
        glm::vec3 getMidPoint() const {
            return (aabbMax + aabbMin) / glm::vec3(2.0);
        }
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec3 tangent;
    };



    class Mesh {
    public:
        Mesh();
        virtual ~Mesh();

        // Getters
        const std::vector<Vertex>& getVertices() const;
        const std::vector<uint32_t>& getIndices() const;
        const Bounds& getBounds() const;
        GLuint getVAO() const;
        GLuint getInstanceVBO() const;

        void setVertices(const std::vector<Vertex>& vertices);
        void setIndices(const std::vector<uint32_t>& indices);
        void setBounds(const Bounds& bounds);
        
        void updateInstanceBuffer(const glm::mat4* matrices, size_t count);

        void setupGPUResources();
    private:
        void setupInstanceBuffer();
    private:
        friend class GeometryRenderPass;
        
        // CPU data
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        Bounds mBounds;

        // GPU resources
        GLuint mVAO; // Vertex Array Object
        GLuint mVBO; // Vertex Buffer Object
        GLuint mEBO; // Element Buffer Object
        GLuint mInstanceVBO; // Instance Buffer Object for matrices

    };

    class Model : public Asset {
    public:
        Model(const std::string& id, const std::filesystem::path& path);
        virtual ~Model();

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "model";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        // Getters
        const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;
        const Bounds& getBounds() const;
        
        // Mesh access methods
        std::shared_ptr<Mesh> getMesh(uint32_t index) const;
        uint32_t getMeshCount() const;
        
        void addMesh(const std::shared_ptr<Mesh>& mesh);
        void clearMeshes();
        
        void postLoad() override;

    private:
        std::vector<std::shared_ptr<Mesh>> mMeshes;
        Bounds mBounds;
    };
}