#include "common.hlsl"

cbuffer objectBuffer : register(b1)
{
	float4x4 ObjectToScreen;
#ifndef GEOMETRY_ONLY
	float4x3 ObjectToView;
	float3 Color;
#endif
}

struct VSInput
{
	float3 m_position : POSITION;
};

struct PSInput
{
	float4 m_position	: SV_POSITION;
};

void vsMain( VSInput input, out PSInput output )
{
	output.m_position = mul( float4( input.m_position, 1.f ), ObjectToScreen );
}

#ifndef GEOMETRY_ONLY
void psMain(PSInput input, out float4 output : SV_TARGET0 )
{
	output = float4( Color, 1.f );
#else
void psMain(PSInput input)
{
#endif
}