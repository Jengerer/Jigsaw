#include "JigsawPiece.h"

JigsawPiece::MeshMap JigsawPiece::PermutationMeshes;

JigsawPiece::JigsawPiece(const Vector2& position, const JigsawMesh::Permutation& permutation)
	: mPosition(position)
	, mVertexBuffer(0)
	, mIndexBuffer(0)
	, mObject(0)
{
}

// Prepare all valid permutations.
void JigsawPiece::GeneratePermutations()
{
	const JigsawMesh::PermutationLess comparator;
	const JigsawMesh::Permutation endPermutation = {
		JigsawMesh::eFLAT, JigsawMesh::eFLAT, JigsawMesh::eFLAT, JigsawMesh::eFLAT
	};
	JigsawMesh::Permutation permutation = {
		JigsawMesh::eOUTWARD, JigsawMesh::eOUTWARD, JigsawMesh::eOUTWARD, JigsawMesh::eOUTWARD
	};

	// Completely flat box is invalid permutation, so end at it.
	while (comparator(permutation, endPermutation)) {
		JigsawMesh* mesh = new JigsawMesh();
		mesh->Generate(permutation);
		PermutationMeshes[permutation] = mesh;
		permutation = JigsawMesh::NextPermutation(permutation);
	}
}
