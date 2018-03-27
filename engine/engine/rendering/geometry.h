#pragma once
#include "vertexFormats.h"

struct SGeometryData
{
	TArray<SSimpleObjectVertexFormat> m_vertices;
	TArray<UINT16> m_indices;
};

struct SGeometry
{
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	ID3D12Resource* m_indicesRes;
	ID3D12Resource* m_vertexRes;

	FORCE_INLINE void Release()
	{
		if ( m_indicesRes )
		{
			m_indexBufferView.BufferLocation = UINT64_MAX;
			m_indexBufferView.SizeInBytes = UINT_MAX;
			m_indexBufferView.Format = DXGI_FORMAT_FORCE_UINT;

			m_indicesRes->Release();
			m_indicesRes = nullptr;
		}

		if ( m_vertexRes )
		{
			m_vertexBufferView.BufferLocation = UINT64_MAX;
			m_vertexBufferView.SizeInBytes = UINT_MAX;
			m_vertexBufferView.StrideInBytes = UINT_MAX;

			m_vertexRes->Release();
			m_vertexRes = nullptr;
		}
	}

	SGeometry()
	{
		m_indexBufferView.BufferLocation = UINT64_MAX;
		m_indexBufferView.SizeInBytes = UINT_MAX;
		m_indexBufferView.Format = DXGI_FORMAT_FORCE_UINT;

		m_vertexBufferView.BufferLocation = UINT64_MAX;
		m_vertexBufferView.SizeInBytes = UINT_MAX;
		m_vertexBufferView.StrideInBytes = UINT_MAX;

		m_indicesRes = nullptr;
		m_vertexRes = nullptr;
	}
};
