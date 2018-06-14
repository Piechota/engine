#include "../headers.h"
#include "../rendering/render.h"

void CComponentStaticMeshManager::FillRenderData() const
{
	SCommonRenderData renderData;
	renderData.m_topology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderData.m_drawType = EDrawType::DrawIndexedInstanced;
	renderData.m_instancesNum = 1;

	renderData.m_verticesStart = 0;
	renderData.m_indicesStart = 0;

	Matrix4x4 const worldToScreen = GViewObject.m_camera.m_worldToScreen;
	Matrix4x4 const worldToView = GViewObject.m_camera.m_worldToView;
	UINT const renderComponentsNum = m_renderComponents.Size();
	for ( UINT i = 0; i < renderComponentsNum; ++i )
	{
		SComponentTransform const transform = GComponentTransformManager.GetComponentNoCheck( m_renderComponents[ i ].m_transformID );
		SComponentStaticMesh const staticMesh = GetComponentNoCheck( m_renderComponents[ i ].m_staticMeshID );

		Matrix4x4 const objectToWorld = Matrix4x4::GetTranslateRotationSize( transform.m_position, transform.m_rotation, transform.m_scale );

		Matrix4x4 tObjectToView = Math::Mul( objectToWorld, worldToView );
		Matrix4x4 tObjectToScreen = Math::Mul( objectToWorld, worldToScreen );
		tObjectToView.Transpose();
		tObjectToScreen.Transpose();

		renderData.m_indicesNum = GRender.GetGeometryIndicesNum( staticMesh.m_geometryID );
		renderData.m_geometryID = staticMesh.m_geometryID;
		renderData.m_shaderID = staticMesh.m_shaderID;

		renderData.m_texturesOffset = GRender.GetTexturesOffset();
		renderData.m_texturesNum = ARRAYSIZE( staticMesh.m_textureID );
		for ( UINT texture = 0; texture < ARRAYSIZE( staticMesh.m_textureID ); ++texture )
		{
			GRender.AddTextureID( staticMesh.m_textureID[ texture ] );
		}

		CConstBufferCtx const cbCtx = GRender.GetConstBufferCtx( renderData.m_cbOffset, staticMesh.m_shaderID );
		cbCtx.SetParam( &tObjectToScreen,		sizeof( tObjectToScreen ),	Hash( "ObjectToScreen" ) );
		cbCtx.SetParam( &tObjectToView,		3 * sizeof( Vec4 ),				Hash( "ObjectToView" ) );
		cbCtx.SetParam( &staticMesh.m_color,	sizeof( Vec4 ),				Hash( "Color" ) );
		cbCtx.SetParam( &staticMesh.m_tiling,	sizeof( Vec2 ),				Hash( "Tiling" ) );

		GRender.AddCommonRenderData( renderData, staticMesh.m_layer );
	}
}

void CComponentStaticMeshManager::FillEnviroParticleRenderData() const
{
	SShadowRenderData renderData;
	renderData.m_topology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderData.m_drawType = EDrawType::DrawIndexedInstanced;
	renderData.m_instancesNum = 1;

	renderData.m_verticesStart = 0;
	renderData.m_indicesStart = 0;

	renderData.m_shaderID = EShaderType::ST_OBJECT_DRAW_GEOMETRY_ONLY;

	Matrix4x4 const worldToScreen = GViewObject.m_enviroParticleWorldToScreen;
	UINT const renderComponentsNum = m_renderComponents.Size();
	for ( UINT i = 0; i < renderComponentsNum; ++i )
	{
		SComponentStaticMesh const staticMesh = GetComponentNoCheck( m_renderComponents[ i ].m_staticMeshID );
		if ( staticMesh.m_layer == RL_OPAQUE )
		{
			SComponentTransform const transform = GComponentTransformManager.GetComponentNoCheck( m_renderComponents[ i ].m_transformID );

			Matrix4x4 objectToWorld = Matrix4x4::GetTranslateRotationSize( transform.m_position, transform.m_rotation, transform.m_scale );
			Matrix4x4 tObjectToScreen = Math::Mul( objectToWorld, worldToScreen );
			tObjectToScreen.Transpose();

			renderData.m_indicesNum = GRender.GetGeometryIndicesNum( staticMesh.m_geometryID );
			renderData.m_geometryID = staticMesh.m_geometryID;

			CConstBufferCtx const cbCtx = GRender.GetConstBufferCtx( renderData.m_cbOffset, staticMesh.m_shaderID );
			cbCtx.SetParam( &tObjectToScreen, sizeof( tObjectToScreen ), Hash( "ObjectToScreen" ) );

			GRender.AddEnviroParticleRenderData( renderData );
		}
	}
}
