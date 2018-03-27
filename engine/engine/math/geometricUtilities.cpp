#include "../math.h"

float Math::DistanceToPoint( Vec3 const& point0, Vec3 const& point1 )
{
	Vec3 const v = point1 - point0;
	float const mag = v.x * v.x + v.y * v.y + v.z * v.z;
	if (0.f < mag)
	{
		return sqrtf(mag);
	}

	return 0.f;
}

float Math::DistanceToLine( Vec3 const& linePoint0, Vec3 const& linePoint1, Vec3 const& point )
{
	Vec3 const P0P1 = linePoint1 - linePoint0;
	Vec3 const P0P = point - linePoint0;
	float const dot = Vec3::Dot( P0P, P0P1 ) / Vec3::Dot( P0P1, P0P1 );

	if ( dot <= 0.f )
	{
		return P0P.GetMagnitude();
	}
	
	if ( 1.f <= dot )
	{
		return DistanceToPoint( linePoint1, point );
	}

	return ( P0P - P0P1 * dot ).GetMagnitude();
}

float Math::DistanceToTriangle( Vec3 const* triangle, Vec3 const& point )
{
	Vec3 const v10 = triangle[1] - triangle[0]; 
	Vec3 const v21 = triangle[2] - triangle[1]; 
	Vec3 const v02 = triangle[0] - triangle[2]; 
	Vec3 const p0 = point - triangle[0];
	Vec3 const p1 = point - triangle[1];
	Vec3 const p2 = point - triangle[2];

	Vec3 const nor = Vec3::Cross( v10, v02 );
	Vec3 const v10CNor = Vec3::Cross( v10, nor );
	Vec3 const v21CNor = Vec3::Cross( v21, nor );
	Vec3 const v02CNor = Vec3::Cross( v02, nor );

	float const dot0 = Vec3::Dot( v10CNor, p0 );
	float const dot1 = Vec3::Dot( v21CNor, p1 );
	float const dot2 = Vec3::Dot( v02CNor, p2 );

	float distance = 0.f;
	if ( dot0 < 0.f || dot1 < 0.f || dot2 < 0.f )
	{
		Vec3 const edge0 = v10 * Math::Clamp( Vec3::Dot( v10, p0 ) / Vec3::Dot( v10, v10 ), 0.f, 1.f ) - p0;
		Vec3 const edge1 = v21 * Math::Clamp( Vec3::Dot( v21, p1 ) / Vec3::Dot( v21, v21 ), 0.f, 1.f ) - p1;
		Vec3 const edge2 = v02 * Math::Clamp( Vec3::Dot( v02, p2 ) / Vec3::Dot( v02, v02 ), 0.f, 1.f ) - p2;

		float const edge0Sq = edge0.GetMagnitudeSq();
		float const edge1Sq = edge1.GetMagnitudeSq();
		float const edge2Sq = edge2.GetMagnitudeSq();

		distance = min( min( edge0Sq, edge1Sq ), edge2Sq );
	}
	else
	{
		distance = Vec3::Dot( nor, p0 ) * Vec3::Dot( nor, p0 ) / Vec3::Dot( nor, nor );
	}

	return sqrtf( distance );
}
