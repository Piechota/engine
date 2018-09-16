#include "../headers.h"
#include "physicsUtilities.h"
#include "../geometryLoader.h"

struct SConvexHalfEdge
{
	UINT m_tailID;
	UINT m_prevID;
	UINT m_nextID;
	UINT m_twinID;
	UINT m_faceID;
};
POD_TYPE(SConvexHalfEdge)

struct SConvexFace
{
	TArray< Vec3 > m_coflictList;
	Vec3 m_normal;
	UINT m_edgeID;
};

static inline void SetFace( UINT const faceID, UINT const edge0, UINT const edge1, UINT const edge2, TArray< Vec3 > const& vertices, TArray<SConvexHalfEdge>& convexHalfEdges, TArray<SConvexFace>& convexFaces )
{
	convexFaces[ faceID ].m_edgeID = edge0;

	convexHalfEdges[ edge0 ].m_faceID = faceID;
	convexHalfEdges[ edge1 ].m_faceID = faceID;
	convexHalfEdges[ edge2 ].m_faceID = faceID;

	Vec3 const v0 = vertices[ convexHalfEdges[ edge0 ].m_tailID ];
	Vec3 const v1 = vertices[ convexHalfEdges[ edge1 ].m_tailID ];
	Vec3 const v2 = vertices[ convexHalfEdges[ edge2 ].m_tailID ];

	convexFaces[ faceID ].m_normal = Vec3::Cross( v1 - v0, v2 - v0 ).GetNormalized();

	convexHalfEdges[ edge0 ].m_nextID = edge1;
	convexHalfEdges[ edge1 ].m_nextID = edge2;
	convexHalfEdges[ edge2 ].m_nextID = edge0;

	convexHalfEdges[ edge0 ].m_prevID = edge2;
	convexHalfEdges[ edge1 ].m_prevID = edge0;
	convexHalfEdges[ edge2 ].m_prevID = edge1;
}

static void FillConflictList( UINT const* facesIDs, UINT const facesCount, TArray<Vec3> const& testVertices, TArray<Vec3> const& vertices, TArray<SConvexHalfEdge> const& edges, TArray<SConvexFace>& faces )
{
	UINT const verticesCount = testVertices.Size();

	for ( UINT vertexID = 0; vertexID < verticesCount; ++vertexID )
	{
		Vec3 const vertex = testVertices[ vertexID ];
		UINT closestfaceID = UINT_MAX;
		float minDistance = FLT_MAX;
		for ( UINT faceID = 0; faceID < facesCount; ++faceID )
		{
			SConvexFace const& face = faces[ facesIDs[ faceID ] ];
			if ( Vec3::Dot( face.m_normal, vertex - vertices[ edges[ face.m_edgeID ].m_tailID ] ) < 0.f )
			{
				UINT const endEdgeID = face.m_edgeID;
				UINT edgeID = endEdgeID;
				do
				{
					SConvexHalfEdge const& edge = edges[ edgeID ];
					Vec3 const v0 = vertices[ edge.m_tailID ];
					Vec3 const v1 = vertices[ edges[ edge.m_twinID ].m_tailID ];

					float const distance = Math::DistanceToLine( v0, v1, vertex );
					if ( distance < minDistance )
					{
						minDistance = distance;
						closestfaceID = facesIDs[ faceID ];
					}

					edgeID = edge.m_nextID;
				} while ( edgeID != endEdgeID );
			}
		}

		if ( closestfaceID != UINT_MAX )
		{
			faces[ closestfaceID ].m_coflictList.Add( vertex );
		}
	}
}

static void GetInitialHull( TArray<Vec3> const& vertices, TArray<SConvexHalfEdge>& convexHalfEdges, TArray<SConvexFace>& convexFaces )
{
	UINT const verticesCount = vertices.Size();
	
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float minZ = FLT_MAX;

	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
	float maxZ = -FLT_MAX;

	UINT extrimVertices[ 6 ];
	for ( UINT i = 0; i < verticesCount; ++i )
	{
		Vec3 const vertex = vertices[ i ];
		if ( vertex.x < minX )
		{
			extrimVertices[ 0 ] = i;
			minX = vertex.x;
		}

		if ( vertex.y < minY )
		{
			extrimVertices[ 1 ] = i;
			minY = vertex.y;
		}

		if ( vertex.z < minZ )
		{
			extrimVertices[ 2 ] = i;
			minZ = vertex.z;
		}

		if ( maxX < vertex.x )
		{
			extrimVertices[ 3 ] = i;
			maxX = vertex.x;
		}

		if ( maxY < vertex.y )
		{
			extrimVertices[ 4 ] = i;
			maxY = vertex.y;
		}

		if ( maxZ < vertex.z )
		{
			extrimVertices[ 5 ] = i;
			maxZ = vertex.z;
		}
	}

	Vec3 triangelPoints[ 3 ];
	UINT pointsID[ 4 ];

	float maxDistance = 0.f;
	for ( UINT v = 0; v < 6; ++v )
	{
		Vec3 const vertex = vertices[ extrimVertices[ v ] ];
		for ( UINT i = v + 1; v < 6; ++v )
		{
			Vec3 const testVertex = vertices[ extrimVertices[ i ] ];
			float const distance = Math::DistanceToPoint( vertex, testVertex );
			if ( maxDistance < distance )
			{
				maxDistance = distance;

				triangelPoints[0] = vertex;
				triangelPoints[1] = testVertex;

				pointsID[ 0 ] = v;
				pointsID[ 1 ] = i;
			}
		}
	}

	maxDistance = 0.f;
	for ( UINT i = 0; i < verticesCount; ++i )
	{
		Vec3 const testVertex = vertices[ i ];
		float const distance = Math::DistanceToLine( triangelPoints[0], triangelPoints[1], testVertex );
		if ( maxDistance < distance )
		{
			maxDistance = distance;
			pointsID[ 2 ] = i;
			triangelPoints[ 2 ] = testVertex;
		}
	}

	maxDistance = 0.f;
	for ( UINT i = 0; i < verticesCount; ++i )
	{
		float const distance = Math::DistanceToTriangle( triangelPoints, vertices[ i ] );
		if ( maxDistance < distance )
		{
			maxDistance = distance;
			pointsID[ 3 ] = i;
		}
	}

	for ( UINT i = 0; i < 3; ++i )
	{
		UINT const id0 = 2 * i + 0;
		UINT const id1 = 2 * i + 1;

		convexHalfEdges[ id0 ].m_tailID = pointsID[ i % 3 ];
		convexHalfEdges[ id1 ].m_tailID = pointsID[ ( i + 1 ) % 3 ];

		convexHalfEdges[ id0 ].m_twinID = id1;
		convexHalfEdges[ id1 ].m_twinID = id0;
	}

	for ( UINT i = 0; i < 3; ++i )
	{
		UINT const id0 = 6 + 2 * i + 0;
		UINT const id1 = 6 + 2 * i + 1;

		convexHalfEdges[ id0 ].m_tailID = pointsID[ i % 3 ];
		convexHalfEdges[ id1 ].m_tailID = pointsID[ 3 ];

		convexHalfEdges[ id0 ].m_twinID = id1;
		convexHalfEdges[ id1 ].m_twinID = id0;
	}

	SetFace( 0, 0, 2, 4, vertices, convexHalfEdges, convexFaces );
	SetFace( 1, 5, 10, 7, vertices, convexHalfEdges, convexFaces );
	SetFace( 2, 6, 9, 1, vertices, convexHalfEdges, convexFaces );
	SetFace( 3, 8, 11, 3, vertices, convexHalfEdges, convexFaces );

	constexpr UINT facesIDs[] = { 0, 1, 2, 3 };
	FillConflictList( facesIDs, 4, vertices, vertices, convexHalfEdges, convexFaces );
}

static void FindHorizon( UINT const faceID, TArray<Vec3> const& vertices, TArray<SConvexHalfEdge> const& convexHalfEdges, TArray<SConvexFace> const& convexFaces, TArray<UINT>& horizonEdges )
{
	SConvexFace const& face = convexFaces[ faceID ];
	horizonEdges.Clear();

	float maxDistance = 0.f;
	UINT furthestVertex = 0;

	Vec3 const faceVertex0 = vertices[ convexHalfEdges[ face.m_edgeID ].m_tailID ];
	UINT const verticesCount = face.m_coflictList.Size();
	for ( UINT vertexID = 0; vertexID < verticesCount; ++vertexID )
	{
		float const distance = Vec3::Dot( face.m_normal, face.m_coflictList[ vertexID ] - faceVertex0 );
		if ( maxDistance < distance )
		{
			maxDistance = distance;
			furthestVertex = vertexID;
		}
	}

	std::set<UINT> visitedFaces;
	visitedFaces.insert( faceID );
	TArray<UINT> edgesIDs;
	edgesIDs.Add( face.m_edgeID );
	while ( edgesIDs.Size() )
	{
		UINT const edgeID = edgesIDs[ edgesIDs.Size() - 1 ];
		SConvexHalfEdge const edge = convexHalfEdges[ edgeID ];
		SConvexHalfEdge const twinEdge = convexHalfEdges[ edge.m_twinID ];

	}
}

void PhysicsUtilities::GenerateConvexFromVertices(TArray<Vec3> const& vertices)
{
	UINT const verticesNum = vertices.Size();
	TArray<SConvexHalfEdge> convexHalfEdges;
	convexHalfEdges.Resize( 2 * ( 3 * verticesNum - 6 ) );
	TArray<SConvexFace> convexFaces;
	convexFaces.Resize( 2 * verticesNum - 4 );

	GetInitialHull( vertices, convexHalfEdges, convexFaces );
}