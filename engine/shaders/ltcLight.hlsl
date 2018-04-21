#include "common.hlsl"
#include "ltcCommon.hlsl"

cbuffer objectBuffer : register(b1)
{
	float4x3 ObjectToView;
	float3 Color;
}

Texture2D DiffTex			: register( t0 );
Texture2D NormalTex			: register( t1 );
Texture2D EmissiveRoughness : register( t2 );
Texture2D DepthTex			: register( t3 );
Texture2D LTCAmp			: register( t4 );
Texture2D LTCMat			: register( t5 );
#ifdef TEXTURE
Texture2D FilteredTexture	: register( t6 );
#endif

SamplerState Sampler : register(s2);

struct VSToPS
{

	float4 m_position : SV_POSITION;
	float2 m_uv : TEXCOORD;
	nointerpolation float3 verticesVS[ 4 ] : TEXCOORD1;
};

void vsMain( float2 position : POSITION, out VSToPS output )
{
	output.m_position = float4( position, 0.f, 1.f );
	output.m_uv = position;

	[unroll]
	for ( uint i = 0; i < 4; ++i )
	{
		output.verticesVS[ i ] = mul( PlaneVertices[ i ], ObjectToView ).xyz;
	}
}

float4 psMain( VSToPS input ) : SV_TARGET
{
	float4 normalVS = NormalTex.Load( int3( input.m_position.xy, 0 ) );
	float4 baseColorMetalness = DiffTex.Load( int3( input.m_position.xy, 0 ) );
	float4 emissiveRoughness = EmissiveRoughness.Load( int3( input.m_position.xy, 0 ) );
	float3 positionVS = GetPositionVS( DepthTex, PerspectiveValues, input.m_uv, input.m_position.xy );
	float3 f0 = lerp( 0.04f, baseColorMetalness.rgb, baseColorMetalness.a );

	float3 eyeVector = normalize( -positionVS );

	float3 normal = normalize( normalVS.xyz * 2.f - 1.f );
	float roughness = emissiveRoughness.a;

	float2 ltcUV = LTC_Coords( dot( normal, eyeVector ), roughness );
	float3x3 minV = LTC_Matrix( LTCMat, Sampler, ltcUV );
	float2 schlick = LTCAmp.Sample( Sampler, ltcUV ).xy;

#ifdef TEXTURE
	float3 ltcSpecular = LTC_Evaluate_Specular( FilteredTexture, Sampler, normal, eyeVector, positionVS, minV, input.verticesVS, false );
	float3 ltcDiffuse = LTC_Evaluate_Diffuse( FilteredTexture, Sampler, normal, eyeVector, positionVS, input.verticesVS, false );
#else
	float ltcSpecular = LTC_Evaluate_Specular( normal, eyeVector, positionVS, minV, input.verticesVS, false );
	float ltcDiffuse = LTC_Evaluate_Diffuse( normal, eyeVector, positionVS, input.verticesVS, false );
#endif

	float3 color = Color * (( baseColorMetalness.rgb * ltcDiffuse )+( ltcSpecular * (f0 * schlick.x + (1.f - f0) * schlick.y ))) / ( 2.f * M_PI );

	return float4( color, 1.f );
}