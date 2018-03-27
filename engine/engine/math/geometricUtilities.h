#pragma once

namespace Math
{
	extern float DistanceToPoint( Vec3 const& point0, Vec3 const& point1 );
	extern float DistanceToLine( Vec3 const& linePoint0, Vec3 const& linePoint1, Vec3 const& point );
	extern float DistanceToTriangle( Vec3 const* triangle, Vec3 const& point );
};