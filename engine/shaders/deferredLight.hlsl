#include "common.hlsl"

cbuffer objectBuffer : register(b1)
{
#ifdef POINT
	float3 LightPos; 
	float2 Attenuation; //x - invRadius, y - fade
#endif

	float3 Color;

#ifdef AMBIENT
	float3 AmbientColor;
#endif

#ifdef DIRECT
	float3 LightDirVS; 
#endif
}

Texture2D DiffTex			: register( t0 );
Texture2D NormalTex			: register( t1 );
Texture2D EmissiveRoughness : register( t2 );
Texture2D DepthTex			: register( t3 );

float3 SchlickF( float3 f0, float voh )
{
	return f0 + ( 1.f - f0 ) * pow( 2.f, ( -5.55473f * voh - 6.98316 ) * voh );
}

float DisneyNDF( float roughness, float noh )
{
	float roughness4 = roughness * roughness;
	roughness4 *= roughness4;

	float den = noh * noh * ( roughness4 - 1.f ) + 1.f;
	return roughness4 / ( M_PI * den * den );
}

float SchlickG( float roughness, float nol, float nov )
{
	float k = roughness + 1.f;
	k = k * k * ( 1.f / 8.f );

	float gL = nol * ( 1.f - k ) + k;
	float gV = nov * ( 1.f - k ) + k;

	return rcp(gL * gV);
}

float3 SpecularTerm( float3 f0, float roughness, float3 normal, float3 view, float3 halfDir, float3 lightDir )
{
	float noh = saturate( dot( normal, halfDir ) );
	float nov = saturate( dot( normal, view ) );
	float voh = saturate( dot( view, halfDir ) );
	float nol = saturate( dot( normal, lightDir ) );

	return SchlickF( f0, voh ) * ( 0.25f * DisneyNDF( roughness, noh ) * SchlickG( roughness, nol, nov ) );
}

float3 DiffuseTerm( float3 baseColor )
{
	return baseColor * ( 1.f / M_PI );
}

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

	float3 lightDirVS = float3(0.f, 0.f, 1.f);
	float3 color = 0.f;
	float nol = 0.f;
	float att = 1.f;

#ifdef POINT
	lightDirVS = LightPos - positionVS;
	float distance = saturate( length( lightDirVS ) * Attenuation.x );
	float b = -Attenuation.y - 1.f;
	att = distance * ( distance * b + Attenuation.y ) + 1.f;
#endif

#ifdef DIRECT
	lightDirVS = LightDirVS;
#endif

	float3 diffuse = DiffuseTerm( baseColorMetalness.rgb );

#if defined(POINT) || defined(DIRECT)
	float3 eyeVector = normalize( -positionVS );
	float3 normLigtDirVS = normalize( lightDirVS );
	float3 halfVec = normalize( eyeVector + normLigtDirVS );

	float3 normal = normalize( normalVS.xyz * 2.f - 1.f );
	float roughness = emissiveRoughness.a;

	float3 f0 = lerp( 0.04f, baseColorMetalness.rgb, baseColorMetalness.a );
	float3 specular = SpecularTerm( f0, roughness, normal, eyeVector, halfVec, lightDirVS );

	nol = saturate( dot( normLigtDirVS, normal ) );
	color = Color * ( att * nol ) * ( specular + diffuse );
#endif

#ifdef AMBIENT
	color += diffuse * AmbientColor + emissiveRoughness.rgb;
#endif

	return float4( color, 1.f );
}