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

namespace ComponentStaticMeshManager
{
	DECLARE_COMPONENT(SComponentStaticMesh)

	extern void RegisterRenderComponents(SComponentHandle const transformHandle, SComponentHandle const staticMeshHandle);
	extern void FillRenderData();
	extern void FillEnviroParticleRenderData();
}
