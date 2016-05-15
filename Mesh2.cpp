#include "Common.h"
#include "Mesh2.h"
#include <cassert>
#include <tuple>

Mesh2::Mesh2(Polygon2& polygon)
{
	mPolygon = std::move(polygon);
}

// Check if a point is to the left or right of a segment.
bool Mesh2::IsVertexLeft(const Vector2& start, const Vector2& end, const Vector2& point)
{
	const float determinantX = (end.x - start.x) * (point.y - start.y);
	const float determinantY = (end.y - start.y) * (point.x - start.x);
	return (determinantX > determinantY);
}

// Return whether a point at a given index is a reflex.
bool Mesh2::IsVertexReflex(const Polygon2& polygon, const TriangulateNode& node)
{
	const Vertices2& vertices = polygon.GetVertices();
    const Vector2& previous = vertices[node.mPrevious->mIndex];
    const Vector2& current = vertices[node.mIndex];
    const Vector2& next = vertices[node.mNext->mIndex];
    return IsVertexLeft(previous, current, next);
}

// Return whether one point is contained in a triangle centered at another non-reflex point.
bool Mesh2::IsVertexInEar(const Polygon2& polygon, const TriangulateNode& earNode, const Vector2& vertex)
{
	const Vertices2& vertices = polygon.GetVertices();
    const Vector2& previous = vertices[earNode.mIndex];
    const Vector2& center = vertices[earNode.mPrevious->mIndex];
    const Vector2& next = vertices[earNode.mNext->mIndex];
    const bool leftAB = IsVertexLeft(previous, center, vertex);
    const bool leftBC = IsVertexLeft(center, next, vertex);
    const bool leftCA = IsVertexLeft(next, previous, vertex);
    return (leftAB == leftBC) && (leftBC == leftCA);
}

// Check if an ear centered at the given node can be removed.
bool Mesh2::CanRemoveEar(const Polygon2& polygon, const TriangulateNode& node)
{
    // Can only clip non-reflex points.
	const Vertices2& vertices = polygon.GetVertices();
    if (IsVertexReflex(polygon, node)) {
        return false;
    }

    // Check that no reflex points are inside this ear.
	const TriangulateNode* const start = node.mNext->mNext;
	const TriangulateNode* const end = node.mPrevious;
    for (const TriangulateNode* p = start; p != end; p = p->mNext) {
        // Only check reflex points.
        if (!IsVertexReflex(polygon, *p)) {
            continue;
        }

        // Fail if we find one reflex inside the ear.
        const U32 nodeIndex = p->mIndex;
        const Vector2& vertex = vertices[nodeIndex];
        if (IsVertexInEar(polygon, node, vertex)) {
            return false;
        }
    }
    return true;
}

// Get the cosine of the smallest angle.
float Mesh2::GetMaximumCosine(const Polygon2& polygon, const TriangulateNode& node)
{
	const Vertices2& vertices = polygon.GetVertices();
    const Vector2& a = vertices[node.mIndex];
    const Vector2& b = vertices[node.mPrevious->mIndex];
    const Vector2& c = vertices[node.mNext->mIndex];

    // Check AB/AC.
	const Vector2& ab = Math::Normalize2(b - a);
	const Vector2& ac = Math::Normalize2(c - a);
    const float cosineAbAc = Math::Dot2(ab, ac);

    // Check BA/BC.
	const Vector2& ba = -ab;
	const Vector2& bc = Math::Normalize2(c - b);
	const float cosineBaBc = Math::Dot2(ba, bc);

    // Check CA/CB.
	const Vector2& ca = -ac;
	const Vector2& cb = -bc;
	const float cosineCaCb = Math::Dot2(ca, cb);

    // Get the biggest one.
	const float maximumAB = Math::Maximum(cosineAbAc, cosineBaBc);
	const float result = Math::Maximum(cosineCaCb, maximumAB);
    return result;
}

// Create a mesh from a polygon.
void Mesh2::Triangulate()
{
    // Allocate space for node list.
	const Vertices2& vertices = mPolygon.GetVertices();
	const U32 count = static_cast<U32>(vertices.size());
    const U32 triangleCount = count - Math::TriangleToVerticesOffset;
    const U32 indexCount = triangleCount * Math::VerticesPerTriangle;
	mIndices.reserve(indexCount);

	// Create list for triangulating.
	using TriangulateNodes = std::vector<TriangulateNode>;
	TriangulateNodes nodes(count);

    // Build list.
	for (U32 i = 0; i < count; ++i) {
		const U32 previousIndex = (i + (count - 1)) % count;
		const U32 nextIndex = (i + 1) % count;
		TriangulateNode& current = nodes[i];
		current.mIndex = i;
		current.mNext = &nodes[nextIndex];
		current.mPrevious = &nodes[previousIndex];
	}

    // Now start clipping ears.
	TriangulateNode* head = &nodes.front();
    for (U32 clipsRemaining = triangleCount; clipsRemaining != 0; --clipsRemaining) {
        // Find the candidate with the largest minimum angle.
		TriangulateNode* node = head;
		std::tuple<float, TriangulateNode*> lowest = std::make_tuple(1.f, nullptr);
        for (U32 c = clipsRemaining; c != 0; --c, node = node->mNext) {
            if (CanRemoveEar(mPolygon, *node)) {
                // If this triangle's smallest angle has a smaller cosine, take over.
                const float currentMaximumCosine = GetMaximumCosine(mPolygon, *node);
                if (currentMaximumCosine <= std::get<F32>(lowest)) {
					lowest = std::make_tuple(currentMaximumCosine, node);
					break;
                }
            }
        }

        // Remove from list.
		TriangulateNode* lowestNode = std::get<TriangulateNode*>(lowest);
        assert(lowestNode != nullptr);
        TriangulateNode* next = lowestNode->mNext;
        TriangulateNode* previous = lowestNode->mPrevious;
        next->mPrevious = previous;
        previous->mNext = next;
        lowestNode->mNext = nullptr;
        lowestNode->mPrevious = nullptr;
        if (lowestNode == head) {
            head = next;
        }

        // Add the indices to triangle index list.
		mIndices.push_back(previous->mIndex);
		mIndices.push_back(lowestNode->mIndex);
		mIndices.push_back(next->mIndex);
    }
}
