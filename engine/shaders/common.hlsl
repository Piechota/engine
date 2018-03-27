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