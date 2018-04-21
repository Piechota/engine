
//https://eheitzresearch.wordpress.com/415-2/

float2 LTC_Coords(float cosTheta, float roughness)
{
	float theta = acos(cosTheta);
	float2 coords = float2(roughness, theta/(0.5f*M_PI));

	const float LUT_SIZE = 32.0f;
	// scale and bias coordinates, for correct filtered lookup
	coords = coords*(LUT_SIZE - 1.0f)/LUT_SIZE + 0.5f/LUT_SIZE;

	return coords;
}

float3x3 LTC_Matrix(Texture2D texLCTMat, SamplerState samp, float2 uv)
{
	// load inverse matrix
	float4 values = texLCTMat.Sample(samp, uv);
	float3x3 Minv;
	Minv[ 0 ] = float3( 1.f, 0.f, values.w );
	Minv[ 1 ] = float3( 0.f, values.z, 0.f );
	Minv[ 2 ] = float3( values.y, 0.f, values.x );

	return Minv;
}

float IntegrateEdge(float3 v1, float3 v2)
{
	float cosTheta = dot(v1, v2);
	cosTheta = min( 0.9999, max( -0.9999, cosTheta ) );

	float theta = acos(cosTheta);    
	float res = cross(v1, v2).z * theta / sin(theta);

	return res;
}

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
	Puv.x = dot(V1, P)*inv_dot_V1_V1 - dot_V1_V2*inv_dot_V1_V1*Puv.y ;

	// LOD
	float d = abs( planeDistxPlaneArea ) / pow( planeAreaSquared, 0.75f );

	//float texSize = 1024.f; //stained glass
	float texSize = 4048.f; //lena
	return filteredTexture.SampleLevel(samp, float2(0.25f, 0.25f) + 0.5f * Puv, log(texSize*d)/log(3.0f) ).rgb;
}

#ifdef TEXTURE
float3 LTC_Evaluate_Diffuse( Texture2D filteredTexture, SamplerState samp, float3 N, float3 V, float3 P, float3 points[4], bool twoSided )
#else
float LTC_Evaluate_Diffuse( float3 N, float3 V, float3 P, float3 points[4], bool twoSided )
#endif
{
	// construct orthonormal basis around N
	float3 T1, T2;
	T1 = normalize(V - N*dot(V, N));
	T2 = cross(N, T1);

	// rotate area light in (T1, T2, R) basis
	float3x3 Minv;
	Minv[ 0 ] = T1;
	Minv[ 1 ] = T2;
	Minv[ 2 ] = N;

	// polygon (allocate 5 vertices for clipping)
	float3 L[5];
	L[0] = mul(Minv, points[0] - P);
	L[1] = mul(Minv, points[1] - P);
	L[2] = mul(Minv, points[2] - P);
	L[3] = mul(Minv, points[3] - P);
	L[4] = L[3]; // avoid warning

#ifdef TEXTURE
	float3 textureLight = FetchDiffuseFilteredTexture( filteredTexture, samp, L[ 0 ], L[ 1 ], L[ 2 ], L[ 3 ] );
#endif

	int n;
	ClipQuadToHorizon(L, n);

	// integrate
	float sum = 0.0;
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
		sum = twoSided ? abs( sum ) : max( 0.0, -sum );
	}
#ifdef TEXTURE
	return sum * textureLight;
#else
	return sum;
#endif
} 

#ifdef TEXTURE
float3 LTC_Evaluate_Specular( Texture2D filteredTexture, SamplerState samp, float3 N, float3 V, float3 P, float3x3 Minv, float3 points[4], bool twoSided )
#else
float LTC_Evaluate_Specular( float3 N, float3 V, float3 P, float3x3 Minv, float3 points[4], bool twoSided )
#endif
{
	// construct orthonormal basis around N
	float3 T1, T2;
	T1 = normalize(V - N*dot(V, N));
	T2 = cross(N, T1);

	// rotate area light in (T1, T2, R) basis
	float3x3 tn;
	tn[ 0 ] = T1;
	tn[ 1 ] = T2;
	tn[ 2 ] = N;
	Minv = mul(Minv, tn);

	// polygon (allocate 5 vertices for clipping)
	float3 L[5];
	L[0] = mul(Minv, points[0] - P);
	L[1] = mul(Minv, points[1] - P);
	L[2] = mul(Minv, points[2] - P);
	L[3] = mul(Minv, points[3] - P);
	L[4] = L[3]; // avoid warning

#ifdef TEXTURE
	float3 textureLight = FetchDiffuseFilteredTexture( filteredTexture, samp, L[ 0 ], L[ 1 ], L[ 2 ], L[ 3 ] );
#endif

	int n;
	ClipQuadToHorizon(L, n);

	// integrate
	float sum = 0.0;
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
		sum = twoSided ? abs( sum ) : max( 0.0, -sum );
	}

#ifdef TEXTURE
	return sum * textureLight;
#else
	return sum;
#endif
}

static const float4 PlaneVertices[] =
{
	float4( -1.f, 0.f, 1.f, 1.f ),
	float4( 1.f, 0.f, 1.f, 1.f ),
	float4( 1.f, 0.f, -1.f, 1.f ),
	float4( -1.f, 0.f, -1.f, 1.f )
};