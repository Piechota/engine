#pragma once

struct SComponentStaticMesh
{
	Vec2 m_tiling;
	Byte m_geometryInfoID;
	Byte m_shaderID;
	Byte m_textureID[ SCommonRenderData::MAX_TEXTURES_NUM ];

	Byte m_layer;
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
