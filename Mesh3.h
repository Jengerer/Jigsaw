#pragma once

#include "common.h"

// Class for storing 3D mesh vertices and index buffer.
class Mesh3
{
public:
	Mesh3() = default;
	~Mesh3() = default;

	// Allocate a mesh for a certain number of vertices/indices.
	void Reserve(U32 vertexCount, U32 indexCount)
	{
		mVertices.reserve(vertexCount);
		mIndices.reserve(indexCount);
	}

	void AddVertex(const Vector3& vertex)
	{
		mVertices.push_back(vertex);
	}

	inline void AddIndex(U32 index)
	{
		mIndices.push_back(index);
	}

	inline const Vertices3& GetVertices() const
	{
		return mVertices;
	}
	
	inline const Indices& GetIndices() const
	{
		return mIndices;
	}

private:
	Vertices3 mVertices;
	Indices mIndices;
};
