#pragma once

#include "Common.h"
#include "JigsawMesh.h"
#include <windows.h>
#include <gl/gl.h>
#include <map>

// Piece that stores all information for simulating and rendering a jigsaw piece.
class JigsawPiece
{
public:
	JigsawPiece(const Vector2& position, const JigsawMesh::Permutation& permutation);
	~JigsawPiece() = default;

public:
	// Prepare all valid permutations.
	static void GeneratePermutations();

private:
	using MeshMap = std::map<JigsawMesh::Permutation, JigsawMesh*, JigsawMesh::PermutationLess>;
	static MeshMap PermutationMeshes;

private:
	Vector2 mPosition;

	// Rendering parameters.
	GLuint mVertexBuffer;
	GLuint mIndexBuffer;
	GLuint mObject;
};