#include "common.hlsl"
cbuffer objectBuffer : register(b1)
{
	float4 Color;
	float2 Cutoff;
}

Texture2D DistTex : register(t0);
SamplerState Sampler : register(s1);

struct VSInput
{
	float2 m_position : POSITION;
	float2 m_uv : TEXCOORD;
};

struct PSInput
{
	float4 m_position	: SV_POSITION;
	float2 m_uv			: TEXCOORD;
};

void vsMain( VSInput input, out PSInput output )
{
	output.m_position = float4( input.m_position, 1.f, 1.f );
	output.m_uv = input.m_uv;
}

float4 psMain(PSInput input) : SV_TARGET0
{
	float dist = DistTex.Sample(Sampler, input.m_uv).r;
	clip( dist - Cutoff.x );
	float alpha = smoothstep( Cutoff.x, Cutoff.y, dist );
	return float4( Color.rgb, Color.a * alpha );
}