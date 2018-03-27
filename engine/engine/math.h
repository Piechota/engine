#pragma once

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))

#include <math.h>


namespace MathConsts
{
	float constexpr PI = 3.14159265359f;
	float constexpr DegToRad = PI / 180.f;
}
float RandFloat();

#include "math\vector.h"
#include "math\matrix.h"
#include "math\quaternion.h"

Vec2 operator*( float const a, Vec2 const v );
Vec3 operator*( float const a, Vec3 const v );
Vec3 operator*( Vec3 const v0, Vec3 const v1 );
Vec3 operator-( Vec3 const v );

#include "math\geometricUtilities.h"

namespace Math
{
	template< typename T >
	T Clamp( T const a, T const minValue, T const maxValue )
	{
		return min( maxValue, max( minValue, a ) );
	}

	Matrix3x3 Mul(Matrix3x3 const& a, Matrix3x3 const& b);
	Vec4 Mul( Vec4 const& vector, Matrix4x4 const& matrix );
	Matrix4x4 Mul( Matrix4x4 const& matrixA, Matrix4x4 const& matrixB );
	float Snap( float const value, float const snap );
	Vec3 Snap( Vec3 const value, Vec3 const snap );
	Vec3 Snap( Vec3 const value, float const snap );
	float Clamp01( float const value );
	Vec3 Abs( Vec3 const value );
}

