#pragma once
#include "Asset.h"
#include "glad/glad.h"
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <pugixml.hpp>

namespace Strike {

    class GraphicsAsset;

    /**
     * @brief Axis-aligned bounding box (AABB) structure.
     */
    struct Bounds {
        glm::vec3 aabbMax, aabbMin;
        Bounds() {
            float minNum = std::numeric_limits<float>::lowest();
            float maxNum = std::numeric_limits<float>::max();
            aabbMin = glm::vec3(maxNum, maxNum, maxNum);
            aabbMax = glm::vec3(minNum, minNum, minNum);
        }

        /**
         * @brief Construct a bounding box from a single point.
         * @param p The point to initialize the bounding box.
         */
        Bounds(const glm::vec3& p) {
            aabbMax = p;
            aabbMin = p;
        }

        /**
         * @brief Construct a bounding box from min and max points.
         *  @param min Minimum point.
         *  @param max Maximum point.
         */
        Bounds(const glm::vec3& min, const glm::vec3& max) {
            aabbMax = max;
            aabbMin = min;
        }
        /**
         * @brief Construct a bounding box that encompasses three points.
         * @param a First point.
         * @param b Second point.
         * @param c Third point.
         */
        Bounds(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
            aabbMax = glm::max(glm::max(a, b), c);
            aabbMin = glm::min(glm::min(a, b), c);
        }

        /**
         * @brief Expand the bounding box to include another bounding box.
         * @param bounds The other bounding box to include.
         */
        void unionBounds(const Bounds& bounds) {
            aabbMax = glm::max(aabbMax, bounds.aabbMax);
            aabbMin = glm::min(aabbMin, bounds.aabbMin);
        }

        /**
         * @brief Calculate the surface area of the bounding box.
         * @return Surface area of the bounding box.
         */

        float area() const {
            glm::vec3 e = aabbMax - aabbMin;
            return e.x * e.y + e.y * e.z + e.z * e.x;
        }

        /**
         * @brief Calculate the midpoint of the bounding box.
         * @return Midpoint of the bounding box.
         */
        glm::vec3 getMidPoint() const {
            return (aabbMax + aabbMin) / glm::vec3(2.0);
        }


        /**
         * @brief Calculate the size (extents) of the bounding box.
         * @return A vec3 representing the width, height, and depth of the bounding box.
         */
        glm::vec3 getSize() const {
            return aabbMax - aabbMin;
        }
    };

    /**
     * @brief Vertex structure for 3D models.
     */
    struct Vertex {
        glm::vec3 position; 
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec3 tangent;
    };


    /**
     * @brief Mesh class representing a single mesh in a 3D model.
     * @note Used within Model assets.
     */
    class Mesh {
    public:
        Mesh();
        virtual ~Mesh();

        /**
         * @brief Returns the mesh vertices.
         * @return Const reference to the internal vertex buffer.
         * @warning The reference becomes invalid if the mesh is destroyed.
         */
        const std::vector<Vertex>& getVertices() const;

        /**
         * @brief Returns the mesh indices.
         * @return Const reference to the internal index buffer.
         * @warning The reference becomes invalid if the mesh is destroyed.
         */
        const std::vector<uint32_t>& getIndices() const;

        /**
         * @brief Returns the axis-aligned bounding box (AABB) of the mesh.
         * @return Const reference to the mesh bounds.
         * @warning The reference becomes invalid if the mesh is destroyed.
         */
        const Bounds& getBounds() const;

        /**
         * @brief Get the Vertex Array Object (VAO) for rendering.
         * @return OpenGL VAO handle.
         */
        GLuint getVAO() const;

        /**
         * @brief Get the Instance Buffer Object (VBO) for instanced rendering.
         * @return OpenGL VBO handle.
         */
        GLuint getInstanceVBO() const;


    private:
        friend class Model;
        friend class ModelLoader;
        friend class GeometryRenderPass;
        friend class ShadowMapPass;

        void updateInstanceBuffer(const glm::mat4* matrices, size_t count);

        void setVertices(const std::vector<Vertex>& vertices);
        void setIndices(const std::vector<uint32_t>& indices);
        void setBounds(const Bounds& bounds);
        void setupInstanceBuffer();
        void setupGPUResources();
    private:
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

    /**
     * @brief 3D Model asset type.
     * @note Loaded via AssetManager by ModelLoader.
     */
    class Model : public Asset {
    public:
        /**
         * @brief Construct a new Model object.
         * @param id Unique identifier for the model in AssetManager.
         * @param path File path to the model asset.
         * @note This does not load the model asset; for loading use AssetManager.
         */
        Model(const std::string& id, const std::filesystem::path& path);
        virtual ~Model();

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "model";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return Model::getStaticTypeName();
        }

        /**
         * @brief Get the meshes contained in the model.
         * @return Const reference to the vector of meshes.
         * @warning The reference becomes invalid if the model is destroyed.
         */
        const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;

        /**
         * @brief Get the overall bounds of the model.
         * @return Const reference to the model bounds.
         * @warning The reference becomes invalid if the model is destroyed.
         */
        const Bounds& getBounds() const;
        

        /**
         * @brief Get a specific mesh by index.
         * @param index The index of the mesh to retrieve.
         * @return Shared pointer to the requested mesh. Returns nullptr if index is out of range.
         */
        const std::shared_ptr<Mesh> getMesh(uint32_t index) const;

        /**
         * @brief Get the number of meshes in the model.
         * @return Number of meshes.
         */
        uint32_t getMeshCount() const;

    protected:
        void postLoad() override;
        
    private:
        friend class ModelParser;
        friend class ModelLoader;
        void addMesh(const std::shared_ptr<Mesh>& mesh);
        void clearMeshes();


    private:
        std::vector<std::shared_ptr<Mesh>> mMeshes;
        Bounds mBounds;
    };
}