#pragma once

struct SComponentStaticMesh
{
	Vec3 m_color;
	Vec2 m_tiling;
	UINT16 m_geometryID;
	Byte m_shaderID;
	UINT16 m_textureID[ SCommonRenderData::MAX_TEXTURES_NUM ];

	Byte m_layer;

	void Init()
	{
		m_color = Vec3::ONE;
		m_tiling = Vec2::ONE;
		m_geometryID = 0;
		m_shaderID = 0;
		m_layer = 0;

		memset( m_textureID, 0, sizeof( m_textureID ) );
	}
};
POD_TYPE( SComponentStaticMesh )


class CComponentStaticMeshManager : public TComponentContainer< SComponentStaticMesh, EComponentType::CT_StaticMesh >
{
private:
	struct SRenderComponents
	{
		UINT32 m_transformID;
		UINT32 m_staticMeshID;
	};
	TArray< SRenderComponents > m_renderComponents;

public:
	void RegisterRenderComponents( SComponentHandle const transformHandle, SComponentHandle const staticMeshHandle )
	{
		ASSERT( transformHandle.m_type == EComponentType::CT_Transform );
		ASSERT( staticMeshHandle.m_type == EComponentType::CT_StaticMesh );
		m_renderComponents.Add( { transformHandle.m_index, staticMeshHandle.m_index } );
	}

	void FillRenderData() const;
	void FillEnviroParticleRenderData() const;
};
