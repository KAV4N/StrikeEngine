#pragma once


namespace StrikeEngine {
	class RawModel
	{
	public:
		RawModel(int vaoID, int vertexCount)
			: m_vaoID(vaoID), m_VertexCount(vertexCount)
		{

		}

		inline int GetVaoID() { return m_vaoID; };
		inline int GetVertexCount() { return m_VertexCount; };

	private:
		int m_vaoID;
		int m_VertexCount;
	};
}