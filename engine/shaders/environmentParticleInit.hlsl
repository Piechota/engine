#include "common.hlsl"
#include "environmentParticleCommon.hlsl"

cbuffer objectBuffer : register(b1)
{
	float3 Velocity;
	float2 InitSize;
	float2 SizeRand;
	float2 VelocityOffsetRand;
	float2 SpeedRand;
	float2 TileSize;
	uint2 TileNum;
	uint Seed;
	uint ParticleNum;
}
RWStructuredBuffer< SEnvironmentParticle > Particles : register( u0 );

float FloatRandStatic( uint seed, float minVal, float maxVal )
{
	seed = (seed << 13) ^ seed;
	float v = (1.0f - ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
	return  (maxVal * ( v + 1.f ) + minVal * ( 1.f - v ) ) * 0.5f;
}

uint UIntRandStatic( uint seed, uint minVal, uint maxVal )
{
	return minVal + ( maxVal - minVal ) * FloatRandStatic( seed, 0.f, 1.f );
}

float3 Float3RandStatic(uint seed, float minVal, float maxVal)
{
	return float3( FloatRandStatic( seed, minVal, maxVal ), FloatRandStatic( seed + 1, minVal, maxVal ), FloatRandStatic( seed + 2, minVal, maxVal ) );
}

[numthreads(64, 1, 1)]
void csMain( uint3 dispatchID : SV_DispatchThreadID )
{
	if ( dispatchID.x < ParticleNum )
	{
		uint seedOffset = Seed + 10 * dispatchID.x;
		uint2 coord = uint2( dispatchID.x % 5, dispatchID.x / 5 );
		Particles[ dispatchID.x ].m_position = Float3RandStatic( seedOffset, 0.f, 1.f );
		seedOffset += 3;
		Particles[ dispatchID.x ].m_size = InitSize * FloatRandStatic( seedOffset, SizeRand.x, SizeRand.y );
		++seedOffset;
		float2 velocityRand = normalize( float2( FloatRandStatic( seedOffset, -1.f, 1.f ), FloatRandStatic( seedOffset + 1, -1.f, 1.f ) ) );
		seedOffset += 2;
		velocityRand *= FloatRandStatic( seedOffset, -VelocityOffsetRand.x, VelocityOffsetRand.y );
		++seedOffset;

		Particles[ dispatchID.x ].m_velocity = Velocity;
		Particles[ dispatchID.x ].m_velocity.xz += velocityRand;
		Particles[ dispatchID.x ].m_velocity = normalize( Particles[ dispatchID.x ].m_velocity );
		Particles[ dispatchID.x ].m_uvPosition = TileSize * float2( UIntRandStatic( seedOffset, 0, TileNum.x ), UIntRandStatic( seedOffset + 1, 0, TileNum.y ) );
		seedOffset += 2;

		Particles[ dispatchID.x ].m_speed = FloatRandStatic( seedOffset, SpeedRand.x, SpeedRand.y );
	}
}