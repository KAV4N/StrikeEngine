#include "strikepch.h"
#include "ShaderManager.h"

namespace StrikeEngine {

	ShaderManager* ShaderManager::s_Instance = nullptr;

	void ShaderManager::Create()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new ShaderManager();
		}
	}

	ShaderManager* ShaderManager::Get()
	{
		return s_Instance;
	}

	void ShaderManager::Shutdown()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

	Shader* ShaderManager::GetShader(const std::string& name)
	{
		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
		{
			return it->second;
		}
		
		return nullptr;
	}
}
