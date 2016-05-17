#include "Common.h"
#include "JigsawMesh.h"
#include <cassert>
#include <cmath>

F32 JigsawMesh::Width = JigsawMesh::DefaultWidth;
F32 JigsawMesh::Height = JigsawMesh::DefaultHeight;
F32 JigsawMesh::CircleRadius = JigsawMesh::DefaultCircleRadius;
Vertices2 JigsawMesh::mEndVertices(JigsawMesh::EndVertexCount);

// Next end type iteration.
JigsawMesh::EndType JigsawMesh::NextEndType(JigsawMesh::EndType type)
{
	switch (type)
	{
	case eOUTWARD:
		return eINWARD;
	case eINWARD:
		return eFLAT;
	case eFLAT:
	default:
		return eOUTWARD;
	}
}

// Get the next permutation while iterating.
// Assumes there's a valid next permutation.
JigsawMesh::Permutation JigsawMesh::NextPermutation(const Permutation& permutation)
{
	Permutation result = permutation;

	// Cycle end types and break when we haven't looped.
	const EndType nextTop = NextEndType(result.mTop);
	result.mTop = nextTop;
	if (nextTop != JigsawMesh::eOUTWARD) {
		return result;
	}

	const EndType nextRight = NextEndType(result.mRight);
	result.mRight = nextRight;
	if (nextRight != JigsawMesh::eOUTWARD) {
		return result;
	}

	const EndType nextBottom = NextEndType(result.mBottom);
	result.mBottom = nextBottom;
	if (nextBottom != JigsawMesh::eOUTWARD) {
		return result;
	}

	result.mLeft = NextEndType(result.mLeft);
	return result;
}

// Comparator for permutation.
bool JigsawMesh::PermutationLess::operator()(const JigsawMesh::Permutation& a, const JigsawMesh::Permutation& b) const
{
	if (a.mTop != b.mTop) {
		return (a.mTop < b.mTop);
	}
	if (a.mRight != b.mRight) {
		return (a.mRight < b.mRight);
	}
	if (a.mBottom != b.mBottom) {
		return (a.mBottom < b.mBottom);
	}
	return (a.mLeft < b.mLeft);
}

// Generate the full 3D mesh for a jigsaw piece.
void JigsawMesh::Generate(const Permutation& permutation)
{
	// Generate the face first.
	const Mesh2 faceMesh = GenerateFace(permutation);
	const Polygon2& polygon = faceMesh.GetPolygon();
	const Vertices2& polygonVertices = polygon.GetVertices();
	const U32 faceVertexCount = static_cast<U32>(polygonVertices.size());
	const Indices& faceIndices = faceMesh.GetIndices();
	const U32 faceIndexCount = static_cast<U32>(faceIndices.size());

	// Calculate how many vertices and indices we need.
	const U32 meshVertexCount = 2 * faceVertexCount;
	const U32 meshFacesIndexCount = faceIndexCount * 2;
	const U32 edgeQuadCount = faceVertexCount;
	const U32 edgeIndexCount = (edgeQuadCount * 2) * Math::VerticesPerTriangle;
	const U32 meshIndexCount = meshFacesIndexCount + edgeIndexCount;
	mMesh.Reserve(meshVertexCount, meshIndexCount);

	// Fill vertices as such: front vertices, back vertices.
	const U32 backVertexOffset = faceVertexCount;
	for (const Vector2& vertex : polygonVertices) {
		const Vector3 frontVertex(vertex.x, vertex.y, FrontZ);
		mMesh.AddVertex(frontVertex);
	}
	for (const Vector2& vertex : polygonVertices) {
		const Vector3 backVertex(vertex.x, vertex.y, BackZ);
		mMesh.AddVertex(backVertex);
	}

	// Now copy index buffer for faces.
	assert((faceIndexCount % Math::VerticesPerTriangle) == 0);
	const Indices::const_iterator indicesEnd = faceIndices.end();
	for (Indices::const_iterator i = faceIndices.begin(); i != indicesEnd; i += Math::VerticesPerTriangle) {
		const U32 first = *i;
		const U32 second = *(i + 1);
		const U32 third = *(i + 2);
		mMesh.AddIndex(first);
		mMesh.AddIndex(second);
		mMesh.AddIndex(third);
	}
	for (Indices::const_iterator i = faceIndices.begin(); i != indicesEnd; i += Math::VerticesPerTriangle) {
		const U32 first = *i;
		const U32 second = *(i + 1);
		const U32 third = *(i + 2);

		// Back faces are in reverse triangle order.
		mMesh.AddIndex(first);
		mMesh.AddIndex(third);
		mMesh.AddIndex(second);
	}

	// Now generate quad indices for the outer edges.
	{
		U32 previous = faceVertexCount - 1U;
		for (U32 front = 0; front != faceVertexCount; previous = front, ++front) {
			const U32 previousBack = previous + backVertexOffset;
			const U32 back = front + backVertexOffset;

			// First triangle.
			mMesh.AddIndex(previous);
			mMesh.AddIndex(previousBack);
			mMesh.AddIndex(front);

			// Second triangle.
			mMesh.AddIndex(previousBack);
			mMesh.AddIndex(back);
			mMesh.AddIndex(front);
		}
	}
}

// Generate the unit circle vertices for the bottom jigsaw end.
void JigsawMesh::BuildEndVertices()
{
    // Get the Y value for the first points so we can start at 0.f.
    const float desiredStartY = Math::Clamp((CircleFraction - 0.5f) * 2.f, -1.f, 1.f);
    const float startAngle = Math::ArcCosine(desiredStartY);
    const float offsetY = CircleRadius * -Math::Cosine(startAngle);

    // Now build the side vertices from the right (clockwise).
	Vertices2::iterator front = mEndVertices.begin();
	Vertices2::reverse_iterator back = mEndVertices.rbegin();
    for (U32 i = 0; i < EndSegments; ++i, ++front, ++back) {
        const float percent = static_cast<float>(i) / static_cast<float>(EndSegments);
        const float angle = Math::Lerp(startAngle, Math::Pi, percent);
        const float rightX = CircleRadius * Math::Sine(angle);
        const float vertexY = (CircleRadius * Math::Cosine(angle)) + offsetY;
        *front = Vector2(rightX, vertexY);
        *back = Vector2(-rightX, vertexY);
    }

    // Build fixed middle point.
	*front = Vector2(0.f, -CircleRadius + offsetY);
}

// Transform a direction vector in the bottom end's space into a full end permutation's space.
Vector2 JigsawMesh::TransformToEnd(const Vector2& vector, End end, EndType type)
{
    const float typeFactor = (type == eINWARD) ? -1.f : 1.f;
    switch (end)
    {
    case eTOP:
        return Vector2(-vector.x, -vector.y * typeFactor);
    case eRIGHT:
        return Vector2(-vector.y * typeFactor, vector.x);
    case eBOTTOM:
        return Vector2(vector.x, vector.y * typeFactor);
    case eLEFT:
        return Vector2(vector.y * typeFactor, -vector.x);
    }
    assert(false);
	return Math::Zero2;
}

// Helper to get the middle of a given end piece.
Vector2 JigsawMesh::GetEndCenter(End end)
{
    switch (end)
    {
    case eTOP:
        return Vector2(0.f, 0.5f * Height);
    case eRIGHT:
        return Vector2(0.5f * Width, 0.f);
    case eBOTTOM:
        return Vector2(0.f, -0.5f * Height);
    case eLEFT:
        return Vector2(-0.5f * Width, 0.f);
    }
    assert(false);
    return Math::Zero2;
}

// Helper to write end vertices to an array of vertices.
// Returns the iterator to the next vertex to be written.
void JigsawMesh::WriteEndVertices(Polygon2& polygon, End end, EndType type)
{
    if (type == eFLAT) {
		return;
    }

    const Vector2 endCenter = GetEndCenter(end);
	for (const Vector2& vertex : mEndVertices)
	{
		const Vector2 offset = TransformToEnd(vertex, end, type);
		const Vector2 result = endCenter + offset;
		polygon.AddVertex(result);
	}
}

// Calculate vertex count of a given jigsaw permutation.
U32 JigsawMesh::CalculateVertexCount(const Permutation& permutation)
{
	U32 count = 4U; // Four corners at least.
    if (permutation.mTop != eFLAT) {
        count += EndVertexCount;
    }
    if (permutation.mRight != eFLAT) {
		count += EndVertexCount;
    }
    if (permutation.mBottom != eFLAT) {
		count += EndVertexCount;
    }
    if (permutation.mLeft != eFLAT) {
		count += EndVertexCount;
    }
    return count;
}

// Generate the 2D jigsaw face mesh for a given permutation.
Mesh2 JigsawMesh::GenerateFace(const Permutation& permutation)
{
	Polygon2 polygon;
    const U32 vertexCount = CalculateVertexCount(permutation);
	polygon.Reserve(vertexCount);

    // Add top left vertex and top edge end.
    const Vector2 topLeft(-0.5f * Width, 0.5f * Height);
	polygon.AddVertex(topLeft);
	WriteEndVertices(polygon, eTOP, permutation.mTop);

    // Add top right and right edge end.
    const Vector2 topRight(-topLeft.x, topLeft.y);
	polygon.AddVertex(topRight);
    WriteEndVertices(polygon, eRIGHT, permutation.mRight);

    // Add bottom right and bottom edge end.
    const Vector2 bottomRight(topRight.x, -topRight.y);
	polygon.AddVertex(bottomRight);
	WriteEndVertices(polygon, eBOTTOM, permutation.mBottom);

    // Add bottom left and left end.
    const Vector2 bottomLeft(topLeft.x, bottomRight.y);
	polygon.AddVertex(bottomLeft);
    WriteEndVertices(polygon, eLEFT, permutation.mLeft);

    // Triangulate it.
	Mesh2 result(polygon);
	result.Triangulate();
	return result;
}
