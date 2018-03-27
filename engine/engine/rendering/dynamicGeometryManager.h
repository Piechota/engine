#pragma once

typedef Byte tDynGeometryID;
struct SDynGeometry
{
	void* m_pVertices;
	void* m_pIndices;

	UINT m_maxIndicesNum;
	UINT m_indicesOffset;
	UINT m_byteIndexStride;

	UINT m_maxVerticesNum;
	UINT m_verticesOffset;
	UINT m_byteVertexStride;

	Byte m_geometryID;
};
POD_TYPE(SDynGeometry)

class CDynamicGeometryManager
{
private:
	TArray< SDynGeometry > m_dynGeometry;

public:
	tDynGeometryID AllocateGeometry( UINT const byteVertexSize, UINT const byteVertexStride, UINT const byteIndexSize, DXGI_FORMAT const indicesFormat );
	void ReleaseGeometry( tDynGeometryID const dynGeometryID );
	void GetVerticesForWrite( UINT const verticesNum, tDynGeometryID const dynGeometryID, void*& outData, UINT16& outVerticesOffset );
	void GetIndicesForWrite( UINT const indicesNum, tDynGeometryID const dynGeometryID, void*& outData, UINT16& outIndicesOffset );
	Byte GetGeometryID( tDynGeometryID const dynGeometryID ) const;
	void PreDraw();
};

extern CDynamicGeometryManager GDynamicGeometryManager;
