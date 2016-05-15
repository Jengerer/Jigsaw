#pragma once

#include "Polygon2.h"

class Mesh2
{
public:
	Mesh2(Polygon2& polygon);
	~Mesh2() = default;

	// Run the triangulation algorithm.
	void Triangulate();

	// Get the 2D polygon this mesh was made from.
	inline const Polygon2& GetPolygon() const
	{
		return mPolygon;
	}

	// Get index buffer.
	inline const Indices& GetIndices() const
	{
		return mIndices;
	}

private:
	// Helper struct for triangulating.
	struct TriangulateNode
	{
		U32 mIndex;
		TriangulateNode* mNext;
		TriangulateNode* mPrevious;
	};

	// Check which side of a 2D line segment a vertex is on.
	static bool IsVertexLeft(const Vector2& start, const Vector2& end, const Vector2& point);

	// Check if a given node represents a reflex polygon vertex.
	static bool IsVertexReflex(const Polygon2& polygon, const TriangulateNode& node);

	// Check if a given vertex is inside an ear centered at the given node.
	static bool IsVertexInEar(const Polygon2& polygon, const TriangulateNode& node, const Vector2& vertex);

	// Check if an ear centered at the given node can be removed.
	static bool CanRemoveEar(const Polygon2& polygon, const TriangulateNode& node);

	// Get the maximum cosine (smallest angle) in the ear triangle.
	static float GetMaximumCosine(const Polygon2& polygon, const TriangulateNode& node);

private:
	Polygon2 mPolygon;
	Indices mIndices;
};
