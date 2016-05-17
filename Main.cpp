#include "Common.h"
#include "JigsawMesh.h"
#include "JigsawPiece.h"
#include <cassert>
#include <cstdio>

int main(int argc, char* argv[])
{
	Unused(argc, argv);

    // Generate end vertices.
	JigsawMesh::BuildEndVertices();

	// Generate all permutations.
	JigsawPiece::GeneratePermutations();

	// Build the mesh.
	JigsawMesh piece;
	const JigsawMesh::Permutation permutation = {
		JigsawMesh::eOUTWARD, JigsawMesh::eOUTWARD, JigsawMesh::eINWARD, JigsawMesh::eINWARD
	};
	piece.Generate(permutation);
    system("pause");
    return 0;
}
