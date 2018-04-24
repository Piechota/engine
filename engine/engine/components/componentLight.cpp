#include "../headers.h"
#include "../rendering/render.h"

void CComponentLightManager::FillRenderData() const
{
	Matrix4x4 const worldToView = GViewObject.m_camera.m_worldToView;

	{
		Byte dirLightFlags = Byte( LF_AMBIENT );
		if ( m_directLightColor != Vec3( 0.f, 0.f, 0.f ) )
		{
			dirLightFlags |= Byte( LF_DIRECT );
		}

		SLightRenderData lightRenderData;
		lightRenderData.m_texturesNum = 0;
		lightRenderData.m_lightShader = dirLightFlags;

		Vec3 const directionLightVS = Math::MulVectorOrtho( m_directLightDir, worldToView );

		CConstBufferCtx const cbCtx = GRender.GetLightConstBufferCtx( lightRenderData.m_cbOffset, dirLightFlags );
		if ( dirLightFlags & Byte( LF_DIRECT ) )
		{
			cbCtx.SetParam( &directionLightVS,		sizeof( directionLightVS ),		EShaderParameters::LightDirVS );
			cbCtx.SetParam( &m_directLightColor,	sizeof( m_directLightColor ),	EShaderParameters::Color );
		}
		cbCtx.SetParam( &m_ambientLightColor,		sizeof( m_ambientLightColor ),	EShaderParameters::AmbientColor );

		GRender.AddLightRenderData( lightRenderData );
	}

	SLightRenderData lightRenderData;
	UINT const renderComponentsNum = m_renderComponents.Size();
	GRender.LightRenderDataReserveNext( renderComponentsNum );

	for ( UINT i = 0; i < renderComponentsNum; ++i )
	{
		SComponentTransform const transform = GComponentTransformManager.GetComponentNoCheck( m_renderComponents[ i ].m_transformID );
		SComponentLight const light = GetComponentNoCheck( m_renderComponents[ i ].m_lightID );

		lightRenderData.m_lightShader = light.m_lightShader;
		lightRenderData.m_texturesNum = 0;

		if ( light.m_lightShader & LF_TEXTURE )
		{
			lightRenderData.m_texturesNum = 1;
			lightRenderData.m_texturesOffset = GRender.GetTexturesOffset();
			GRender.AddTextureID( light.m_textureID );
		}

		Matrix4x4 const objectToWorld = Matrix4x4::GetTranslateRotationSize( transform.m_position, transform.m_rotation, transform.m_scale );
		Matrix4x4 tObjectToView = Math::Mul( objectToWorld, worldToView );
		tObjectToView.Transpose();

		Vec3 const positionVS = Math::MulPositionOrtho( transform.m_position, worldToView );
		Vec2 const attenuation( 1.f / light.m_radius, light.m_fade );

		CConstBufferCtx const cbCtx = GRender.GetLightConstBufferCtx( lightRenderData.m_cbOffset, light.m_lightShader );
		cbCtx.SetParam( &tObjectToView,				3 * sizeof( Vec4 ),						EShaderParameters::ObjectToView );
		cbCtx.SetParam( &positionVS,				sizeof( positionVS ),					EShaderParameters::LightPos );
		cbCtx.SetParam( &light.m_color,				sizeof( light.m_color ),				EShaderParameters::Color );
		cbCtx.SetParam( &attenuation,				sizeof( attenuation ),					EShaderParameters::Attenuation );

		GRender.AddLightRenderData( lightRenderData );
	}
}