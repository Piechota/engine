#include "common.hlsl"
#include "environmentParticleCommon.hlsl"
cbuffer objectBuffer : register(b1)
{
	float4x3 ObjectToWorld;
	float2 UVScale;
	float2 Fade;
	uint2 BoxesNum;
	float Size;
	float Soft;
}
StructuredBuffer< SEnvironmentParticle > Particles : register( t0 );

Texture2D ColorTex : register( t0 );
Texture2D DepthTex : register( t3 );
Texture2D EnviroProjectionTex : register( t4 );
SamplerState Sampler : register(s1);
SamplerState SamplerDepth : register(s2);

float GetLinearDepth( float2 uv )
{
	float depthHW = DepthTex.Load( int3( uv.xy, 0 ) ).r;
	return PerspectiveValues.z / ( depthHW + PerspectiveValues.w );
}

struct VStoPS
{
	float4 m_position : SV_POSITION;
	float3 m_projectionSpace : TEXCOORD0;
	float2 m_uv : TEXCOORD1;
	float m_distance : TEXCOORD2;
	float m_depth : TEXCOORD3;
};

void vsMain(uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID, out VStoPS output ) 
{
	uint particleID = vertexID / 6;
	float3 boxPosition = Size * float3( instanceID % BoxesNum.x, ( instanceID % BoxesNum.y ) / BoxesNum.x, instanceID / BoxesNum.y );
	float4x3 objectToWorld = ObjectToWorld;
	objectToWorld[ 3 ].xyz += boxPosition;

	SEnvironmentParticle particle = Particles[ particleID ];
	float3 velocityOS = particle.m_velocity;
	float3 positionOS = frac( particle.m_position + velocityOS * ( Time * particle.m_speed ) );
	float3 positionWS = mul( float4( positionOS, 1.f ), objectToWorld ).xyz;
	float3 positionToCameraWS = CameraPositionWS - positionWS;
	float3 velocityWS = normalize( mul( float4( velocityOS, 0.f ), objectToWorld ).xyz );
	float3 perpendicularVector = particle.m_size.x * cross( normalize( positionToCameraWS ), velocityWS );
	velocityWS *= particle.m_size.y;

	float3 verticesPositions[] =
	{
		-perpendicularVector - velocityWS,
		-perpendicularVector + velocityWS,
		perpendicularVector - velocityWS,
		perpendicularVector - velocityWS,
		-perpendicularVector + velocityWS,
		perpendicularVector + velocityWS,
	};
	
	float2 verticesUV[] =
	{
		float2( 0.f, 0.f ),
		float2( 0.f, 1.f ),
		float2( 1.f, 0.f ),
		float2( 1.f, 0.f ),
		float2( 0.f, 1.f ),
		float2( 1.f, 1.f ),
	};

	float3 vertexPosition = positionWS + verticesPositions[ vertexID % 6 ];

	output.m_position = mul( float4( vertexPosition, 1.f ), WorldToScreen );
	output.m_projectionSpace = mul( float4( vertexPosition, 1.f ), EnviroProjection );
	output.m_uv = UVScale * verticesUV[ vertexID % 6 ] + particle.m_uvPosition;
	float3 axisDistance = abs( positionToCameraWS );
	output.m_distance = max( max( axisDistance.x, axisDistance.y ), axisDistance.z );
	output.m_depth = output.m_position.z;
}

[earlydepthstencil]
float4 psMain(VStoPS input) : SV_TARGET0
{
	float2 uvPosition = input.m_projectionSpace.xy;
	float depth = EnviroProjectionTex.Sample( SamplerDepth, uvPosition ).r;
	clip( depth - input.m_projectionSpace.z);

	float linearDepth = GetLinearDepth( input.m_position.xy );
	float soft = smoothstep( 0.f, Soft, linearDepth - input.m_depth );
	float fade = soft * smoothstep( Fade.x, Fade.y, input.m_distance );
	clip( fade - 0.002f );

	return fade * ColorTex.Sample( Sampler, input.m_uv );
}
