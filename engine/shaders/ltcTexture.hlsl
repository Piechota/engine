cbuffer buffer : register( b0 )
{
	float InvDstTextureSize;
	float DstTextureSize;
	float MipMap;
	int MaxKernelRadius;
};

Texture2D SourceTex : register(t0);
StructuredBuffer<float> GaussKernel : register(t1);
SamplerState Sampler : register(s0);

RWTexture2D<float4> DestinyTex : register( u0 );

[numthreads(16, 16, 1)]
void csMain( uint3 dispatchID : SV_DispatchThreadID )
{
	uint2 dstCoord = dispatchID.xy;

	if ( uint( DstTextureSize ) <= dstCoord.x || uint( DstTextureSize ) <= dstCoord.y )
	{
		return;
	}

	int inTexKernelRadius = int(MipMap);
	int maxKernelDim = 2 * max( MaxKernelRadius, inTexKernelRadius ) + 1;

	float2 distanceXY = saturate( abs( 2.f * float2( dstCoord.xy ) * InvDstTextureSize - 1.f ) - 0.75f );
	float distance = max( distanceXY.x, distanceXY.y );
	distance = 0.f < distance ? ( distance + 0.15f ) : distance;
	distance = ceil( DstTextureSize * distance );

	int kernelRadius = min( MaxKernelRadius, max( inTexKernelRadius, int( distance ) ) );
	int kernelDim = 2 * kernelRadius + 1;
	int kernelSize = kernelDim * kernelDim;

	int kernelOffset = MaxKernelRadius - kernelRadius;

	int sampleX0 = ( int(dstCoord.x) < kernelRadius ? 0 : ( int(dstCoord.x) - kernelRadius ) );
	int sampleY0 = ( int(dstCoord.y) < kernelRadius ? 0 : ( int(dstCoord.y) - kernelRadius ) );

	int sampleX1 = ( min( DstTextureSize, int(dstCoord.x) + kernelRadius + 1 ) );
	int sampleY1 = ( min( DstTextureSize, int(dstCoord.y) + kernelRadius + 1 ) );

	int kernelX0 = kernelRadius + sampleX0 - int(dstCoord.x);
	int kernelY0 = kernelRadius + sampleY0 - int(dstCoord.y);

	int kernelX1 = kernelRadius + sampleX1 - int(dstCoord.x);
	int kernelY1 = kernelRadius + sampleY1 - int(dstCoord.y);

	float sum = 0.f;

	float3 accRGB = 0.f;

	for ( int currentY = sampleY0; currentY < sampleY1; ++currentY )
	{
		int y = ( kernelOffset + kernelY0 + currentY - sampleY0 ) * maxKernelDim;
		for ( int currentX = sampleX0; currentX < sampleX1; ++currentX )
		{
			int x = kernelOffset + kernelX0 + currentX - sampleX0;
			float kernelVal = GaussKernel[ y + x ];

			float2 uv = 1.25f * float2( currentX, currentY ) * InvDstTextureSize - 0.125f;
			float4 color = SourceTex.SampleLevel( Sampler, uv, MipMap );
			accRGB += color.rgb * kernelVal;

			sum += kernelVal;
		}
	}
	accRGB /= sum;
	float4 color = float4( accRGB.rgb, 1.f );
	DestinyTex[ dstCoord.xy ] = color;
}