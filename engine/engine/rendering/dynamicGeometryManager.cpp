#include "../headers.h"
#include "dynamicGeometryManager.h"
#include "render.h"

tDynGeometryID CDynamicGeometryManager::AllocateGeometry( UINT const byteVertexSize, UINT const byteVertexStride, UINT const byteIndexSize, DXGI_FORMAT const indicesFormat )
{
	ID3D12Device* device = GRender.GetDevice();

	D3D12_RESOURCE_DESC descGeometry = {};
	descGeometry.DepthOrArraySize = 1;
	descGeometry.SampleDesc.Count = 1;
	descGeometry.Flags = D3D12_RESOURCE_FLAG_NONE;
	descGeometry.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descGeometry.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descGeometry.MipLevels = 1;
	descGeometry.Width = byteVertexSize;
	descGeometry.Height = 1;
	descGeometry.Format = DXGI_FORMAT_UNKNOWN;

	SGeometry geometry;
	CheckResult(device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descGeometry, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&geometry.m_vertexRes)));
	descGeometry.Width = byteIndexSize;
	CheckResult(device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descGeometry, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&geometry.m_indicesRes)));

	geometry.m_vertexBufferView.BufferLocation	= geometry.m_vertexRes->GetGPUVirtualAddress();
	geometry.m_vertexBufferView.SizeInBytes		= byteVertexSize;
	geometry.m_vertexBufferView.StrideInBytes	= byteVertexStride;

	geometry.m_indexBufferView.BufferLocation	= geometry.m_indicesRes->GetGPUVirtualAddress();
	geometry.m_indexBufferView.SizeInBytes		= byteIndexSize;
	geometry.m_indexBufferView.Format			= indicesFormat;

	tDynGeometryID const dynGeomtryID = tDynGeometryID( m_dynGeometry.Size() );
	m_dynGeometry.Add( SDynGeometry() );

	SDynGeometry& dynGeometry = m_dynGeometry[ m_dynGeometry.Size() - 1 ];
	dynGeometry.m_maxVerticesNum = byteVertexSize / byteVertexStride;
	dynGeometry.m_byteVertexStride = byteVertexStride;
	dynGeometry.m_verticesOffset = 0;

	dynGeometry.m_indicesOffset = 0;
	dynGeometry.m_byteIndexStride = GDXGIFormatsBitsSize[ indicesFormat ] / 8;
	dynGeometry.m_maxIndicesNum = byteIndexSize / dynGeometry.m_byteIndexStride;

	dynGeometry.m_geometryID = GRender.AddGeometry( geometry );

	CheckResult( geometry.m_vertexRes->Map( 0, nullptr, &dynGeometry.m_pVertices ) );
	CheckResult( geometry.m_indicesRes->Map( 0, nullptr, &dynGeometry.m_pIndices ) );

	return dynGeomtryID;
}

void CDynamicGeometryManager::ReleaseGeometry( tDynGeometryID const dynGeometryID )
{
	Byte const geometryID = m_dynGeometry[ dynGeometryID ].m_geometryID;
	SGeometry& geometry = GRender.GetGeometry( geometryID );
	geometry.m_vertexRes->Unmap( 0, nullptr );
	geometry.m_indicesRes->Unmap( 0, nullptr );

	GRender.ReleaseGeometry( m_dynGeometry[ dynGeometryID ].m_geometryID );
}

void CDynamicGeometryManager::GetVerticesForWrite( UINT const verticesNum, tDynGeometryID const dynGeometryID, void*& outData, UINT16& outVerticesOffset )
{
	SDynGeometry& dynGeometry = m_dynGeometry[ dynGeometryID ];
	ASSERT( verticesNum + dynGeometry.m_verticesOffset < dynGeometry.m_maxVerticesNum );

	outVerticesOffset = dynGeometry.m_verticesOffset;
	outData = reinterpret_cast< void* >( &reinterpret_cast< Byte* >( dynGeometry.m_pVertices )[ outVerticesOffset * dynGeometry.m_byteVertexStride ] );

	dynGeometry.m_verticesOffset += verticesNum;
}

void CDynamicGeometryManager::GetIndicesForWrite( UINT const indicesNum, tDynGeometryID const dynGeometryID, void *& outData, UINT16& outIndicesOffset )
{
	SDynGeometry& dynGeometry = m_dynGeometry[ dynGeometryID ];
	ASSERT( indicesNum + dynGeometry.m_indicesOffset < dynGeometry.m_maxIndicesNum );

	outIndicesOffset = dynGeometry.m_indicesOffset;
	outData = reinterpret_cast< void* >( &reinterpret_cast< Byte* >( dynGeometry.m_pIndices )[ outIndicesOffset * dynGeometry.m_byteIndexStride ] );

	dynGeometry.m_indicesOffset += indicesNum;
}

Byte CDynamicGeometryManager::GetGeometryID( tDynGeometryID const dynGeometryID ) const
{
	return m_dynGeometry[ dynGeometryID ].m_geometryID;
}

void CDynamicGeometryManager::PreDraw()
{
	UINT const dynGeometryNum = m_dynGeometry.Size();
	for ( UINT i = 0; i < dynGeometryNum; ++i )
	{
		m_dynGeometry[ i ].m_verticesOffset = 0;
		m_dynGeometry[ i ].m_indicesOffset = 0;
	}
}

CDynamicGeometryManager GDynamicGeometryManager;