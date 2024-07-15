#pragma once
#include "StrikeEngine/Renderer/Texture.h"

namespace StrikeEngine {
	class Model
	{
	public:
		Model(unsigned int vaoID, unsigned int vertexCount, Texture* texture)
			: m_vaoID(vaoID), m_VertexCount(vertexCount), m_Texture(texture)
		{

		}

		Model(unsigned int vaoID, unsigned int vertexCount)
			: m_vaoID(vaoID), m_VertexCount(vertexCount), m_Texture(0)
		{

		}

		inline unsigned int GetVaoID() { return m_vaoID; };
		inline unsigned int GetVertexCount() { return m_VertexCount; };
		inline Texture* GetTextureID() { return m_Texture; };
	private:
		unsigned int m_vaoID;
		unsigned int m_VertexCount;
		Texture* m_Texture;
	};
}
