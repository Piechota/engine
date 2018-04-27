float3 SchlickF( float3 f0, float voh )
{
	return f0 + ( 1.f - f0 ) * pow( 2.f, ( -5.55473f * voh - 6.98316 ) * voh );
}

float DisneyNDF( float roughness, float noh )
{
	float roughness4 = roughness * roughness;
	roughness4 *= roughness4;

	float den = noh * noh * ( roughness4 - 1.f ) + 1.f;
	return roughness4 / ( M_PI * den * den );
}

float SchlickG( float roughness, float nol, float nov )
{
	float k = roughness + 1.f;
	k = k * k * ( 1.f / 8.f );

	float gL = nol * ( 1.f - k ) + k;
	float gV = nov * ( 1.f - k ) + k;

	return rcp(gL * gV);
}

float3 SpecularTerm( float3 f0, float roughness, float3 normal, float3 view, float3 halfDir, float3 lightDir )
{
	float noh = saturate( dot( normal, halfDir ) );
	float nov = saturate( dot( normal, view ) );
	float voh = saturate( dot( view, halfDir ) );
	float nol = saturate( dot( normal, lightDir ) );

	return SchlickF( f0, voh ) * ( 0.25f * DisneyNDF( roughness, noh ) * SchlickG( roughness, nol, nov ) );
}

float3 DiffuseTerm( float3 baseColor )
{
	return baseColor * ( 1.f / M_PI );
}