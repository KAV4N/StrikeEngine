#include "strikepch.h"
#include "Model.h"

namespace StrikeEngine {

	void Model::AddMesh(std::shared_ptr<Mesh> mesh) {
		m_Meshes.push_back(mesh);
	}


} 