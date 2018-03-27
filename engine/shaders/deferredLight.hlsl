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
	float3 LightDirWS; 
#endif
}

Texture2D DiffTex : register( t0 );
Texture2D NormalTex : register( t1 );
Texture2D EmissiveSpecTex : register( t2 );
Texture2D DepthTex : register( t3 );

float GetLinearDepth( float2 uv )
{
	float depthHW = DepthTex.Load( int3( uv.xy, 0 ) ).r;
	return PerspectiveValues.z / ( depthHW + PerspectiveValues.w );
}

float3 GetPositionWS( float2 uv, float2 position )
{
	float linearDepth = GetLinearDepth( position );
	float3 positionWS = float3( uv * PerspectiveValues.xy * linearDepth, linearDepth );
	return mul( float4( positionWS, 1.f ), ViewToWorld ).xyz;
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
	float3 normalWS = normalize(NormalTex.Load( int3( input.m_position.xy, 0 ) ).rgb * 2.f - 1.f);
	float3 baseColor = DiffTex.Load( int3( input.m_position.xy, 0 ) ).rgb;
	float4 emissiveSpec = EmissiveSpecTex.Load( int3( input.m_position.xy, 0 ) );
	float3 positionWS = GetPositionWS( input.m_uv, input.m_position );

	float3 lightDirWS = 0.f;
	float3 color = 0.f;
	float ndl = 0.f;
	float att = 1.f;

#ifdef POINT
	lightDirWS = LightPos - positionWS;
	float distance = saturate( length( lightDirWS ) * Attenuation.x );
	float b = -Attenuation.y - 1.f;
	att = distance * ( distance * b + Attenuation.y ) + 1.f;
#endif

#ifdef DIRECT
	lightDirWS = LightDirWS;
#endif

	float3 eyeVector = normalize( -positionWS );
	float3 normLigtDirWS = normalize( lightDirWS );

	float3 halfVec = normalize( eyeVector + normLigtDirWS );
	float cosAngle = saturate( dot( normalWS, halfVec ) );
	float specularCoef = pow( cosAngle, 80.f );
	float spec = specularCoef * emissiveSpec.a;

	ndl = saturate( dot( normLigtDirWS, normalWS ) );
	color = Color * att * ( ndl * baseColor + spec );

#ifdef AMBIENT
	color += baseColor * AmbientColor + emissiveSpec.rgb;
#endif


	return float4( color, 1.f );
}