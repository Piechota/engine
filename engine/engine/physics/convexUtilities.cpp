#include "../headers.h"
#include "physicsUtilities.h"
#include "../geometryLoader.h"

struct SConvexVertex
{
	Vec3 m_position;
	SConvexVertex* m_prev;
	SConvexVertex* m_next;
};
POD_TYPE(SConvexVertex)

struct SConvexHalfEdge
{
	SConvexVertex* m_tail;
	SConvexHalfEdge* m_prev;
	SConvexHalfEdge* m_next;
	SConvexHalfEdge* m_twin;
	struct SConvexFace* m_face;
};
POD_TYPE(SConvexHalfEdge)

struct SConvexFace
{
	TArray< SConvexVertex > m_coflictList;
	SConvexFace* m_prev;
	SConvexFace* m_next;

	SConvexHalfEdge* m_edge;
};

void PhysicsUtilities::GenerateConvexFromVertices(TArray<Vec3> const& vertices)
{
	UINT const verticesNum = vertices.Size();
	TArray<SConvexVertex> convexVertices( verticesNum );
	TArray<SConvexHalfEdge> convexHalfEdges( 2 * ( 3 * verticesNum - 6 ) );
	TArray<SConvexFace> convexFaces( 2 * verticesNum - 4 );
}