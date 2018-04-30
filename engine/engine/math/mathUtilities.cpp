#include "../math.h"

void Math::GaussianKelner1D( float* const dst, float const weight, int const radius )
{
	int const dim = 2 * radius + 1;

	float const d0 = 1.f / ( weight * sqrt( 2.f * M_PI ) );
	float const d1 = 1.f / ( 2.f * weight * weight );

	for ( int x = -radius; x <= radius; ++x )
	{
		float const dist = float( x ) * float( x );
		dst[ x + radius ] = exp( -dist * d1 ) * d0;
	}
}

void Math::GaussianKelner2D( float* const dst, float const weight, int const radius )
{
	int const dim = 2 * radius + 1;

	float const d0 = 1.f / ( weight * sqrt( 2.f * M_PI ) );
	float const d1 = 1.f / ( 2.f * weight * weight );

	for ( int y = -radius; y <= radius; ++y )
	{
		for ( int x = -radius; x <= radius; ++x )
		{
			float const dist = float( x ) * float( x ) + float( y ) * float( y );
			dst[ (y + radius) * dim + x + radius ] = exp( -dist * d1 ) * d0;
		}
	}
}