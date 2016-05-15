#pragma once

#include "common.h"

// Two dimensional polygon storage.
class Polygon2
{
public:
	Polygon2() = default;
	~Polygon2() = default;

	// Reserve memory for a specific polygon size.
	inline void Reserve(U32 vertexCount)
	{
		mVertices.reserve(vertexCount);
	}

	inline void AddVertex(const Vector2& vertex)
	{
		mVertices.push_back(vertex);
	}

	inline const Vertices2& GetVertices() const
	{
		return mVertices;
	}

private:
	Vertices2 mVertices;
};
