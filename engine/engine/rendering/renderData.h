#pragma once

enum class EDrawType : Byte
{
	DrawInstanced,
	DrawIndexedInstanced,

	DrawInvalid
};

struct SCommonRenderData
{
	enum
	{
		MAX_TEXTURES_NUM = 4
	};

	D3D12_GPU_VIRTUAL_ADDRESS m_cbOffset;
	D3D_PRIMITIVE_TOPOLOGY m_topology;

	UINT16 m_verticesStart;
	UINT16 m_indicesStart;
	UINT16 m_indicesNum;
	UINT16 m_instancesNum;

	UINT8 m_texturesOffset;
	UINT8 m_texturesNum;

	Byte m_geometryID;

	Byte m_shaderID;
	EDrawType m_drawType;
};
POD_TYPE( SCommonRenderData )

struct SShadowRenderData
{
	D3D12_GPU_VIRTUAL_ADDRESS m_cbOffset;
	D3D_PRIMITIVE_TOPOLOGY m_topology;

	UINT16 m_verticesStart;
	UINT16 m_indicesStart;
	UINT16 m_indicesNum;
	UINT16 m_instancesNum;

	Byte m_geometryID;

	Byte m_shaderID;
	EDrawType m_drawType;
};
POD_TYPE( SShadowRenderData )

enum ELightFlags
{
	LF_NONE		= 0,
	LF_DIRECT	= FLAG( 0 ),
	LF_POINT	= FLAG( 1 ),
	LF_AMBIENT	= FLAG( 2 ),

	LF_MAX		= FLAG( 3 )
};

struct SLightRenderData
{
	D3D12_GPU_VIRTUAL_ADDRESS m_cbOffset;
	Byte m_lightShader;
};
POD_TYPE(SLightRenderData)
