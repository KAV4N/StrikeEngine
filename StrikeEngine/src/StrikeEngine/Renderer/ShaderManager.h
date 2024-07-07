#pragma once

#include <string>
#include <unordered_map>

namespace StrikeEngine {

	class Shader {
		// Shader class implementation
	};

	class ShaderManager {
	public:
		static void Create();
		static ShaderManager* Get();
		static void Shutdown();

		Shader* GetShader(const std::string& name);

	private:
		ShaderManager() = default;
		~ShaderManager() = default;

		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator=(const ShaderManager&) = delete;

		static ShaderManager* s_Instance;

		std::unordered_map<std::string, Shader*> m_Shaders;
	};
}
