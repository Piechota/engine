#include "common.hlsl"
cbuffer objectBuffer : register(b1)
{
	float2 ScreenPosition;
	float2 Size;
}

Texture2D Tex : register( t0 );
SamplerState Sampler : register(s1);

struct VSToPS
{
	float4 m_position : SV_POSITION;
	float2 m_uv : TEXCOORD;
};


void vsMain( uint vertexID : SV_VertexID, out VSToPS output )
{
	float2 vertexOffset = float2( ( vertexID & 2 ) ? 1.f : -1.f, ( vertexID & 1 ) ? 1.f : -1.f );
	float2 vertex = ScreenPosition + Size * vertexOffset;

	output.m_position = float4( vertex, 1.f, 1.f );
	output.m_uv = vertexOffset * float2( 0.5f, -0.5f ) + 0.5f;
}

float4 psMain( VSToPS input ) : SV_TARGET
{
	return Tex.Sample( Sampler, input.m_uv );
}