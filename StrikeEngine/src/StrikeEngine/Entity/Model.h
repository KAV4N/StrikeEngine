#pragma once


namespace StrikeEngine {
	class Model
	{
	public:
		Model(int id, int vertexCount)
			: m_Id(id), m_VertexCount(vertexCount)
		{

		}

		inline int GetId() { return m_Id; };
		inline int GetVertexCount() { return m_VertexCount; };

	private:
		int m_Id;
		int m_VertexCount;
	};
}