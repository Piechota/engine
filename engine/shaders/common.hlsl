#define M_PI       3.14159265358979323846f   // pi


cbuffer globalBuffer : register( b0 )
{
	float4x4 EnviroProjection;
	float4x4 WorldToScreen;
	float4x3 ViewToWorld;
	float4 PerspectiveValues;
	float3 CameraPositionWS;
	float DeltaTime;
	float Time;
}

float GetLinearDepth( Texture2D depthTex, float2 uv, float2 perspectiveValues )
{
	float depthHW = depthTex.Load( int3( uv.xy, 0 ) ).r;
	return perspectiveValues.x / ( depthHW + perspectiveValues.y );
}

float3 GetPositionVS( Texture2D depthTex, float4 perspectiveValues, float2 uv, float2 position )
{
	float linearDepth = GetLinearDepth( depthTex, position, perspectiveValues.zw );
	return float3( uv * perspectiveValues.xy * linearDepth, linearDepth );
}