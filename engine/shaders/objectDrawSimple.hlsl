#include "common.hlsl"

cbuffer objectBuffer : register(b1)
{
	float4x4 ObjectToScreen;
#ifndef GEOMETRY_ONLY
	float4x3 ObjectToView;
	float3 Color;
#endif
}

Texture2D DiffTex : register(t0);
SamplerState Sampler : register(s1);

struct VSInput
{
	float3 m_position : POSITION;
#ifdef TEXTURE
	float2 m_uv : TEXCOORD;
#endif
};

struct PSInput
{
	float4 m_position	: SV_POSITION;
#ifdef TEXTURE
	float2 m_uv			: TEXCOORD;
#endif
};

void vsMain( VSInput input, out PSInput output )
{
	output.m_position = mul( float4( input.m_position, 1.f ), ObjectToScreen );
#ifdef TEXTURE
	output.m_uv = input.m_uv;
#endif
}

#ifndef GEOMETRY_ONLY
void psMain(PSInput input, out float4 output : SV_TARGET0 )
{
	output = float4( Color, 1.f );
#ifdef TEXTURE
	output = float4( Color * DiffTex.Sample( Sampler, input.m_uv ).rgb, 1.f );
#endif

#else
void psMain(PSInput input)
{
#endif
}