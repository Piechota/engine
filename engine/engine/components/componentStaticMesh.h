#pragma once

struct SComponentStaticMesh
{
	Vec3 m_color;
	Vec2 m_tiling;
	Byte m_geometryInfoID;
	Byte m_shaderID;
	Byte m_textureID[ SCommonRenderData::MAX_TEXTURES_NUM ];

	Byte m_layer;

	void Init()
	{
		m_color = Vec3::ONE;
		m_tiling = Vec2::ONE;
		m_geometryInfoID = 0;
		m_shaderID = 0;
		m_layer = 0;

		memset( m_textureID, UINT8_MAX, sizeof( m_textureID ) );
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
	void RegisterRenderComponents( UINT32 const transformID, UINT32 const staticMeshID )
	{
		m_renderComponents.Add( { transformID, staticMeshID } );
	}

	void FillRenderData() const;
	void FillEnviroParticleRenderData() const;
};
