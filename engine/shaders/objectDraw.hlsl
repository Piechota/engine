#include "common.hlsl"
cbuffer objectBuffer : register(b1)
{
	float4x4 ObjectToScreen;
	#ifndef GEOMETRY_ONLY
		float4x3 ObjectToView;
		#ifdef TEXTURES
			float2 Tiling;
		#else
			float4 Color;
		#endif
	#endif
}
#if !defined(GEOMETRY_ONLY) && defined(TEXTURES)
Texture2D DiffTex : register(t0);
Texture2D NormTex : register(t1);
Texture2D RoughnessTex : register(t2);
Texture2D MetalnessTex : register(t3);
Texture2D EmissiveTex : register(t4);
SamplerState Sampler : register(s1);
#endif

struct VSInput
{
	float3 m_position	: POSITION;
	float3 m_normal		: NORMAL;
	float3 m_tangent	: TANGENT;
	#ifdef TEXTURES
		float2 m_uv		: TEXCOORD;
	#endif 
};

struct PSInput
{
	#ifdef GEOMETRY_ONLY
		float4 m_position		: SV_POSITION;
	#else
		#ifdef TEXTURES
			float3x3 m_tbn		: TANGENT;
		#else
			float3 m_normal		: NORMAL;
		#endif

		float4 m_position		: SV_POSITION;
		#ifdef TEXTURES
			float2 m_uv			: TEXCOORD;
		#endif
	#endif
};

struct PSOutput
{
	float4 m_diffuse			: SV_TARGET0;
	float4 m_normalVS			: SV_TARGET1;
	float4 m_emissiveRoughness	: SV_TARGET2;
};

void vsMain( VSInput input, out PSInput output )
{
	#ifndef GEOMETRY_ONLY
		#ifdef TEXTURES
			float3 bitangent = cross( input.m_tangent, input.m_normal );

			output.m_tbn[ 0 ] = mul( float4( input.m_tangent, 0.f ), ObjectToView ).xyz;
			output.m_tbn[ 1 ] = mul( float4( bitangent, 0.f ), ObjectToView ).xyz;
			output.m_tbn[ 2 ] = mul( float4( input.m_normal, 0.f ), ObjectToView ).xyz;
			output.m_uv = input.m_uv * Tiling;
		#else
			output.m_normal = mul(float4(input.m_normal, 0.f), ObjectToView).xyz;
		#endif
	#endif

	output.m_position = mul( float4( input.m_position, 1.f ), ObjectToScreen );
}

#ifndef GEOMETRY_ONLY
void psMain(PSInput input, out PSOutput output )
{
	#ifdef TEXTURES
		float4 roughness = RoughnessTex.Sample( Sampler, input.m_uv );
		float4 metalness = MetalnessTex.Sample( Sampler, input.m_uv );

		output.m_normalVS = NormTex.Sample( Sampler, input.m_uv );
		output.m_diffuse = DiffTex.Sample( Sampler, input.m_uv );
		output.m_diffuse.a = metalness.r;

		output.m_normalVS.xyz = mad( output.m_normalVS.xyz, 2.f, -1.f );
		output.m_normalVS.xyz = mul( output.m_normalVS.xyz, input.m_tbn );
		output.m_normalVS.xyz = normalize( output.m_normalVS.xyz );
		output.m_normalVS.xyz = mad( output.m_normalVS.xyz, 0.5f, 0.5f );
		output.m_emissiveRoughness.rgb = EmissiveTex.Sample( Sampler, input.m_uv ).rgb;
		output.m_emissiveRoughness.a = roughness.r;
	#else
		output.m_diffuse = Color;
		output.m_normalVS.xyz = input.m_normal;
		output.m_normalVS.xyz = normalize(output.m_normalVS.xyz);
		output.m_normalVS.xyz = mad(output.m_normalVS.xyz, 0.5f, 0.5f);
		output.m_normalVS.w = 0.f;
		output.m_emissiveRoughness.rgb = 0.f;
		output.m_emissiveRoughness.a = 1.f;
	#endif

#else
void psMain(PSInput input)
{
#endif
}