
//https://eheitzresearch.wordpress.com/415-2/

#include "lightCommon.hlsl"

Texture2D LTCAmp			: register( t4 );
Texture2D LTCMat			: register( t5 );
#ifdef TEXTURE
Texture2D FilteredTexture	: register( t6 );
#endif
SamplerState Sampler		: register(s2);

#define LUT_SIZE  (64.0)
#define LUT_SCALE ((LUT_SIZE - 1.0)/LUT_SIZE)
#define LUT_BIAS  (0.5/LUT_SIZE)

void ClipQuadToHorizon(inout float3 L[5], inout int n)
{
	// detect clipping config
	int config = 0;
	if (L[0].z > 0.0) config += 1;
	if (L[1].z > 0.0) config += 2;
	if (L[2].z > 0.0) config += 4;
	if (L[3].z > 0.0) config += 8;

	// clip
	n = 0;

	if (config == 0)
	{
		// clip all
	}
	else if (config == 1) // V1 clip V2 V3 V4
	{
		n = 3;
		L[1] = -L[1].z * L[0] + L[0].z * L[1];
		L[2] = -L[3].z * L[0] + L[0].z * L[3];
	}
	else if (config == 2) // V2 clip V1 V3 V4
	{
		n = 3;
		L[0] = -L[0].z * L[1] + L[1].z * L[0];
		L[2] = -L[2].z * L[1] + L[1].z * L[2];
	}
	else if (config == 3) // V1 V2 clip V3 V4
	{
		n = 4;
		L[2] = -L[2].z * L[1] + L[1].z * L[2];
		L[3] = -L[3].z * L[0] + L[0].z * L[3];
	}
	else if (config == 4) // V3 clip V1 V2 V4
	{
		n = 3;
		L[0] = -L[3].z * L[2] + L[2].z * L[3];
		L[1] = -L[1].z * L[2] + L[2].z * L[1];
	}
	else if (config == 5) // V1 V3 clip V2 V4) impossible
	{
		n = 0;
	}
	else if (config == 6) // V2 V3 clip V1 V4
	{
		n = 4;
		L[0] = -L[0].z * L[1] + L[1].z * L[0];
		L[3] = -L[3].z * L[2] + L[2].z * L[3];
	}
	else if (config == 7) // V1 V2 V3 clip V4
	{
		n = 5;
		L[4] = -L[3].z * L[0] + L[0].z * L[3];
		L[3] = -L[3].z * L[2] + L[2].z * L[3];
	}
	else if (config == 8) // V4 clip V1 V2 V3
	{
		n = 3;
		L[0] = -L[0].z * L[3] + L[3].z * L[0];
		L[1] = -L[2].z * L[3] + L[3].z * L[2];
		L[2] =  L[3];
	}
	else if (config == 9) // V1 V4 clip V2 V3
	{
		n = 4;
		L[1] = -L[1].z * L[0] + L[0].z * L[1];
		L[2] = -L[2].z * L[3] + L[3].z * L[2];
	}
	else if (config == 10) // V2 V4 clip V1 V3) impossible
	{
		n = 0;
	}
	else if (config == 11) // V1 V2 V4 clip V3
	{
		n = 5;
		L[4] = L[3];
		L[3] = -L[2].z * L[3] + L[3].z * L[2];
		L[2] = -L[2].z * L[1] + L[1].z * L[2];
	}
	else if (config == 12) // V3 V4 clip V1 V2
	{
		n = 4;
		L[1] = -L[1].z * L[2] + L[2].z * L[1];
		L[0] = -L[0].z * L[3] + L[3].z * L[0];
	}
	else if (config == 13) // V1 V3 V4 clip V2
	{
		n = 5;
		L[4] = L[3];
		L[3] = L[2];
		L[2] = -L[1].z * L[2] + L[2].z * L[1];
		L[1] = -L[1].z * L[0] + L[0].z * L[1];
	}
	else if (config == 14) // V2 V3 V4 clip V1
	{
		n = 5;
		L[4] = -L[0].z * L[3] + L[3].z * L[0];
		L[0] = -L[0].z * L[1] + L[1].z * L[0];
	}
	else if (config == 15) // V1 V2 V3 V4
	{
		n = 4;
	}

	if (n == 3)
		L[3] = L[0];
	if (n == 4)
		L[4] = L[0];
}

float2 LTC_Coords(float cosTheta, float roughness)
{
	float2 uv = float2(roughness, sqrt( 1.f - cosTheta) );
	uv = uv*LUT_SCALE + LUT_BIAS;
	return uv;
}

float3x3 LTC_Matrix(float2 uv)
{
	// load inverse matrix
	float4 values = LTCMat.Sample(Sampler, uv);
	float3x3 Minv;
	Minv[ 0 ] = float3( values.x,	0.f,	values.z );
	Minv[ 1 ] = float3( 0.f,		1.f,	0.f );
	Minv[ 2 ] = float3( values.y,	0.f,	values.w );

	return Minv;
}

float3 IntegrateEdgeVecDiffuse(float3 v1, float3 v2 )
{
	float x = dot(v1, v2);
	x = min( 0.9999, max( -0.9999, x ) );
	float y = abs( x );

	float theta_sintheta = 1.5708 + ( -0.879406 + 0.308609 * y ) * y;
	if ( x < 0.f )
	{
		theta_sintheta = M_PI * rsqrt( 1.f - x * x ) - theta_sintheta;
	}

	return cross( v1, v2 ) * theta_sintheta;
}

float3 IntegrateEdgeVec(float3 v1, float3 v2 )
{
	float x = dot(v1, v2);
	x = min( 0.9999, max( -0.9999, x ) );
	float y = abs( x );

	float a = 5.42031 + ( 3.12829 + 0.0902326 * y ) * y;
	float b = 3.45068 + ( 4.18814 + y ) * y;
	float theta_sintheta = a / b;
	if ( x < 0.f )
	{
		theta_sintheta = M_PI * rsqrt( 1.f - x * x ) - theta_sintheta;
	}

	return cross( v1, v2 ) * theta_sintheta;
}

float IntegrateEdge(float3 v1, float3 v2 )
{
	return IntegrateEdgeVec( v1, v2 ).z;
}

#ifdef HIGH_QUALITY
float IntegrateLTC( float3 L[5] )
{
	// integrate
	float sum = 0.0;

	int n;
	ClipQuadToHorizon(L, n);
	if ( n != 0 )
	{
		// project onto sphere
		L[ 0 ] = normalize( L[ 0 ] );
		L[ 1 ] = normalize( L[ 1 ] );
		L[ 2 ] = normalize( L[ 2 ] );
		L[ 3 ] = normalize( L[ 3 ] );
		L[ 4 ] = normalize( L[ 4 ] );


		sum += IntegrateEdge( L[ 0 ], L[ 1 ] );
		sum += IntegrateEdge( L[ 1 ], L[ 2 ] );
		sum += IntegrateEdge( L[ 2 ], L[ 3 ] );
		if ( n >= 4 )
			sum += IntegrateEdge( L[ 3 ], L[ 4 ] );
		if ( n == 5 )
			sum += IntegrateEdge( L[ 4 ], L[ 0 ] );

		// note: negated due to winding order
#ifdef TWO_SIDED
		sum = abs( sum );
#else
		sum = max( 0.0, -sum );
#endif
	}
	
	return sum;
}
#else
float IntegrateLTC( float3 L[4], in float3 points[4], in float3 P, in bool diffuse )
{
	// integrate
	float sum = 0.0;

	float3 dir = points[0].xyz - P;
	float3 lightNormal = cross(points[1] - points[0], points[3] - points[0]);
	bool behind = (0.f <= dot(dir, lightNormal));

	L[0] = normalize(L[0]);
	L[1] = normalize(L[1]);
	L[2] = normalize(L[2]);
	L[3] = normalize(L[3]);

	float3 vsum = 0.0;

	if ( diffuse )
	{
		vsum -= IntegrateEdgeVecDiffuse( L[ 0 ], L[ 1 ] );
		vsum -= IntegrateEdgeVecDiffuse( L[ 1 ], L[ 2 ] );
		vsum -= IntegrateEdgeVecDiffuse( L[ 2 ], L[ 3 ] );
		vsum -= IntegrateEdgeVecDiffuse( L[ 3 ], L[ 0 ] );
	}
	else
	{
		vsum -= IntegrateEdgeVec( L[ 0 ], L[ 1 ] );
		vsum -= IntegrateEdgeVec( L[ 1 ], L[ 2 ] );
		vsum -= IntegrateEdgeVec( L[ 2 ], L[ 3 ] );
		vsum -= IntegrateEdgeVec( L[ 3 ], L[ 0 ] );
	}

	float len = length(vsum);
	float z = vsum.z/len;

	if (behind)
		z = -z;

	float2 uv = float2(z*0.5 + 0.5, len);
	uv = uv*LUT_SCALE + LUT_BIAS;

	float scale = LTCAmp.Sample(Sampler, uv).w;

	sum = len*scale;

#ifndef TWO_SIDED
	if ( behind )
		sum = 0.0;
#endif
	return sum;
}
#endif

float3 FetchDiffuseFilteredTexture(Texture2D filteredTexture, SamplerState samp, float3 p1_, float3 p2_, float3 p3_, float3 p4_)
{
	// area light plane basis
	float3 V1 = p2_ - p1_;
	float3 V2 = p4_ - p1_;
	float3 planeOrtho = (cross(V1, V2));
	float planeAreaSquared = dot(planeOrtho, planeOrtho);
	float planeDistxPlaneArea = dot(planeOrtho, p1_);
	// orthonormal projection of (0,0,0) in area light space
	float3 P = planeDistxPlaneArea * planeOrtho / planeAreaSquared - p1_;

	// find tex coords of P
	float dot_V1_V2 = dot(V1,V2);
	float inv_dot_V1_V1 = 1.0 / dot(V1, V1);
	float3 V2_ = V2 - V1 * dot_V1_V2 * inv_dot_V1_V1;
	float2 Puv;
	Puv.y = dot(V2_, P) / dot(V2_, V2_);
	Puv.x = dot(V1, P)*inv_dot_V1_V1 - dot_V1_V2*inv_dot_V1_V1*Puv.y;

	// LOD
	float d = abs( planeDistxPlaneArea ) / pow( planeAreaSquared, 0.75f ); //	r/sqrt(A)

	float texSize = 1024.f; //stained glass
	//float texSize = 4048.f; //lena
	return filteredTexture.SampleLevel(samp, 0.125f + 0.75f * Puv, log(texSize*d*d) ).rgb; // log(texSize*d)/log(3.0f) ????????????
}

float3 LTC_Evaluate( in float3 N, in float3 V, in float3 P, in float3x3 mat, in float3 points[4], in bool diffuse )
{
#ifdef HIGH_QUALITY
	// polygon (allocate 5 vertices for clipping)
	float3 L[5];
	L[0] = mul(mat, points[0] - P);
	L[1] = mul(mat, points[1] - P);
	L[2] = mul(mat, points[2] - P);
	L[3] = mul(mat, points[3] - P);
	L[4] = L[3]; // avoid warning
#else
	float3 L[4];
	L[0] = mul(mat, points[0] - P);
	L[1] = mul(mat, points[1] - P);
	L[2] = mul(mat, points[2] - P);
	L[3] = mul(mat, points[3] - P);
#endif

#ifdef TEXTURE
	float3 textureLight = FetchDiffuseFilteredTexture( FilteredTexture, Sampler, L[ 0 ], L[ 1 ], L[ 2 ], L[ 3 ] );
#endif

#ifdef HIGH_QUALITY
	float sum = IntegrateLTC( L );
#else
	float sum = IntegrateLTC( L, points, P, diffuse );
#endif

#ifdef TEXTURE
	return sum * textureLight;
#else
	return sum;
#endif
}