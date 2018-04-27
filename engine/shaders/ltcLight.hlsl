#define HIGH_QUALITY
//#define TWO_SIDED

#include "common.hlsl"
#include "ltcCommon.hlsl"

cbuffer objectBuffer : register(b1)
{
	float3 Color;
	float3 Vertices[ 4 ];
}

Texture2D DiffTex			: register( t0 );
Texture2D NormalTex			: register( t1 );
Texture2D EmissiveRoughness : register( t2 );
Texture2D DepthTex			: register( t3 );

struct VSToPS
{
	float4 m_position : SV_POSITION;
	float2 m_uv : TEXCOORD;
};

void vsMain( float2 position : POSITION, out VSToPS output )
{
	output.m_position = float4( position, 0.f, 1.f );
	output.m_uv = position;
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

	float2 ltcUV = LTC_Coords( saturate(dot( normal, eyeVector )), roughness );
	float3x3 ltcMat = LTC_Matrix( ltcUV );
	float2 schlick = LTCAmp.Sample( Sampler, ltcUV ).xy;

	// construct orthonormal basis around N
	float3 T1, T2;
	T1 = normalize(eyeVector - normal*dot(eyeVector, normal));
	T2 = cross(normal, T1);

	// rotate area light in (T1, T2, R) basis
	float3x3 tn = float3x3( T1, T2, normal );
	ltcMat = mul(ltcMat, tn);

	float3 ltcSpecular = LTC_Evaluate( normal, eyeVector, positionVS, ltcMat, Vertices, false );
	float3 ltcDiffuse = LTC_Evaluate( normal, eyeVector, positionVS, tn, Vertices, true );

	float3 diffuse = DiffuseTerm( baseColorMetalness.rgb ) * (1.f/ M_PI) * ltcDiffuse;
	float3 specular = ltcSpecular * ( f0 * schlick.x + ( 1.f - f0 ) * schlick.y );

	float3 color = Color * (diffuse + specular) / ( 2.f * M_PI );

	return float4( color, 1.f );
}