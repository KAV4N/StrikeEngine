#pragma once
#include <vector>
#include <memory>
#include "Mesh.h"

namespace StrikeEngine {
    class ModelManager;
    class Model {
    public:
        Model() = default;
    private:
        friend class ModelManager;
        void AddMesh(std::shared_ptr<Mesh> mesh);
    private:
        std::vector<std::shared_ptr<Mesh>> m_Meshes;
    };

} 