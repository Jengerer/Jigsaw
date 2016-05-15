#pragma once

#include "Common.h"
#include "Mesh2.h"
#include "Mesh3.h"

// Class for storing a specific jigsaw piece permutation.
class JigsawPiece
{
public:
	// Enumeration for jigsaw edge type.
	enum EndType
	{
		eOUTWARD,
		eINWARD,
		eFLAT
	};

	// Structure for representing a permutation of a jigsaw piece.
	struct Permutation
	{
		EndType mTop;
		EndType mRight;
		EndType mBottom;
		EndType mLeft;
	};

public:
	JigsawPiece() = default;
	~JigsawPiece() = default;

	// Generate a mesh for a certain permutation.
	void Generate(const Permutation& permutation);

public:
	// Generate end vertices.
	static void BuildEndVertices();

private:
	// Edge piece parameters.
	static constexpr U32 EndSegments = 5U;
	static constexpr U32 EndVertexCount = ((EndSegments * 2U) + 1U);
	static constexpr F32 CircleFraction = 0.8f;
	static constexpr F32 CircleRadius = 0.5f;

	// Jigsaw parameters.
	static constexpr F32 Depth = 1.f;
	static constexpr F32 FrontZ = 0.5f * Depth;
	static constexpr F32 BackZ = -0.5f * Depth;

	// Temporary jigsaw parameters.
	static constexpr F32 Width = 4.f;
	static constexpr F32 Height = 3.25f;

	// Helper type for transforming end.
	enum End
	{
		eTOP,
		eRIGHT,
		eBOTTOM,
		eLEFT
	};

private:
	// Transform a point from bottom end space into end permutation space.
	static Vector2 TransformToEnd(const Vector2& vector, End end, EndType type);

	// Get a vertex by index for a given end permutation.
	static Vector2 GetEndVertex(U32 index, End end, EndType type);

	// Get local center coordinate for where to put an end piece.
	static Vector2 GetEndCenter(End end);

	// Helper to write end vertices to an array of vertices.
	// Returns the iterator to the next vertex to be written.
	static void WriteEndVertices(Polygon2& polygon, End end, EndType type);

	// Calculate number of vertices for a given jigsaw permutation.
	static U32 CalculateVertexCount(const Permutation& permutation);

	// Generate a 2D mesh for the jigsaw piece face.
	static Mesh2 GenerateFace(const Permutation& permutation);

private:
	static Vertices2 mEndVertices;

private:
	Mesh3 mMesh;
};
