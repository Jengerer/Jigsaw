#include "Common.h"
#include "JigsawPiece.h"
#include <cassert>
#include <cstdio>

int main(int argc, char* argv[])
{
	Unused(argc, argv);

    // Generate end vertices.
	JigsawPiece::BuildEndVertices();

	// Build the mesh.
	JigsawPiece piece;
	const JigsawPiece::Permutation permutation = {
		JigsawPiece::eOUTWARD, JigsawPiece::eOUTWARD, JigsawPiece::eINWARD, JigsawPiece::eINWARD
	};
	piece.Generate(permutation);
    system("pause");
    return 0;
}
