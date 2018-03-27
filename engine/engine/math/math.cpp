#include "../core.h"

Matrix4x4 Matrix4x4::GetTranslateRotationSize( Vec3 const& translate, Quaternion const& rotation, Vec3 const& scale )
{
	Matrix4x4 const rotationM = rotation.ToMatrix4x4();
	Matrix4x4 const translateScale
	(
		scale.x,	0.f,		0.f,		0.f,
		0.f,		scale.y,	0.f,		0.f,
		0.f,		0.f,		scale.z,	0.f,
		translate.x,		translate.y,		translate.z,		1.f
	);
	return Math::Mul( rotationM, translateScale );
}

Matrix3x3 Math::Mul(Matrix3x3 const& a, Matrix3x3 const& b)
{
	return Matrix3x3
	(
		a.m_a00 * b.m_a00 + a.m_a01 * b.m_a10 + a.m_a02 * b.m_a20, a.m_a00 * b.m_a01 + a.m_a01 * b.m_a11 + a.m_a02 * b.m_a21, a.m_a00 * b.m_a02 + a.m_a01 * b.m_a12 + a.m_a02 * b.m_a22,
		a.m_a10 * b.m_a00 + a.m_a11 * b.m_a10 + a.m_a12 * b.m_a20, a.m_a10 * b.m_a01 + a.m_a11 * b.m_a11 + a.m_a12 * b.m_a21, a.m_a10 * b.m_a02 + a.m_a11 * b.m_a12 + a.m_a12 * b.m_a22,
		a.m_a20 * b.m_a00 + a.m_a21 * b.m_a10 + a.m_a22 * b.m_a20, a.m_a20 * b.m_a01 + a.m_a21 * b.m_a11 + a.m_a22 * b.m_a21, a.m_a20 * b.m_a02 + a.m_a21 * b.m_a12 + a.m_a22 * b.m_a22
	);
}

Vec4 Math::Mul(Vec4 const& vector, Matrix4x4 const& matrix)
{
	return Vec4
	(
		vector.x * matrix.m_a00 + vector.y * matrix.m_a10 + vector.z * matrix.m_a20 + vector.w * matrix.m_a30
		, vector.x * matrix.m_a01 + vector.y * matrix.m_a11 + vector.z * matrix.m_a21 + vector.w * matrix.m_a31
		, vector.x * matrix.m_a02 + vector.y * matrix.m_a12 + vector.z * matrix.m_a22 + vector.w * matrix.m_a32
		, vector.x * matrix.m_a03 + vector.y * matrix.m_a13 + vector.z * matrix.m_a23 + vector.w * matrix.m_a33
	);
}
Matrix4x4 Math::Mul(Matrix4x4 const& matrixA, Matrix4x4 const& matrixB)
{
	return Matrix4x4
	(
		matrixA.m_a00 * matrixB.m_a00 + matrixA.m_a01 * matrixB.m_a10 + matrixA.m_a02 * matrixB.m_a20 + matrixA.m_a03 * matrixB.m_a30,
		matrixA.m_a00 * matrixB.m_a01 + matrixA.m_a01 * matrixB.m_a11 + matrixA.m_a02 * matrixB.m_a21 + matrixA.m_a03 * matrixB.m_a31,
		matrixA.m_a00 * matrixB.m_a02 + matrixA.m_a01 * matrixB.m_a12 + matrixA.m_a02 * matrixB.m_a22 + matrixA.m_a03 * matrixB.m_a32,
		matrixA.m_a00 * matrixB.m_a03 + matrixA.m_a01 * matrixB.m_a13 + matrixA.m_a02 * matrixB.m_a23 + matrixA.m_a03 * matrixB.m_a33,

		matrixA.m_a10 * matrixB.m_a00 + matrixA.m_a11 * matrixB.m_a10 + matrixA.m_a12 * matrixB.m_a20 + matrixA.m_a13 * matrixB.m_a30,
		matrixA.m_a10 * matrixB.m_a01 + matrixA.m_a11 * matrixB.m_a11 + matrixA.m_a12 * matrixB.m_a21 + matrixA.m_a13 * matrixB.m_a31,
		matrixA.m_a10 * matrixB.m_a02 + matrixA.m_a11 * matrixB.m_a12 + matrixA.m_a12 * matrixB.m_a22 + matrixA.m_a13 * matrixB.m_a32,
		matrixA.m_a10 * matrixB.m_a03 + matrixA.m_a11 * matrixB.m_a13 + matrixA.m_a12 * matrixB.m_a23 + matrixA.m_a13 * matrixB.m_a33,

		matrixA.m_a20 * matrixB.m_a00 + matrixA.m_a21 * matrixB.m_a10 + matrixA.m_a22 * matrixB.m_a20 + matrixA.m_a23 * matrixB.m_a30,
		matrixA.m_a20 * matrixB.m_a01 + matrixA.m_a21 * matrixB.m_a11 + matrixA.m_a22 * matrixB.m_a21 + matrixA.m_a23 * matrixB.m_a31,
		matrixA.m_a20 * matrixB.m_a02 + matrixA.m_a21 * matrixB.m_a12 + matrixA.m_a22 * matrixB.m_a22 + matrixA.m_a23 * matrixB.m_a32,
		matrixA.m_a20 * matrixB.m_a03 + matrixA.m_a21 * matrixB.m_a13 + matrixA.m_a22 * matrixB.m_a23 + matrixA.m_a23 * matrixB.m_a33,

		matrixA.m_a30 * matrixB.m_a00 + matrixA.m_a31 * matrixB.m_a10 + matrixA.m_a32 * matrixB.m_a20 + matrixA.m_a33 * matrixB.m_a30,
		matrixA.m_a30 * matrixB.m_a01 + matrixA.m_a31 * matrixB.m_a11 + matrixA.m_a32 * matrixB.m_a21 + matrixA.m_a33 * matrixB.m_a31,
		matrixA.m_a30 * matrixB.m_a02 + matrixA.m_a31 * matrixB.m_a12 + matrixA.m_a32 * matrixB.m_a22 + matrixA.m_a33 * matrixB.m_a32,
		matrixA.m_a30 * matrixB.m_a03 + matrixA.m_a31 * matrixB.m_a13 + matrixA.m_a32 * matrixB.m_a23 + matrixA.m_a33 * matrixB.m_a33
	);
}

float Math::Snap( float const value, float const snap )
{
	return snap * floorf( value / snap );
}

Vec3 Math::Snap( Vec3 const value, Vec3 const snap )
{
	return Vec3( Snap( value.x, snap.x ), Snap( value.y, snap.y ), Snap( value.z, snap.z ) );
}

Vec3 Math::Snap( Vec3 const value, float const snap )
{
	return Vec3( Snap( value.x, snap ), Snap( value.y, snap ), Snap( value.z, snap ) );
}

float Math::Clamp01( float const value )
{
	return min( max( value, 0.f ), 1.f );
}

Vec3 Math::Abs( Vec3 const value )
{
	return Vec3( abs( value.x ), abs( value.y ), abs( value.z ) );
}

Vec2 operator*( float const a, Vec2 const v )
{
	return Vec2( a * v.x, a * v.y );
}

Vec3 operator*( float const a, Vec3 const v )
{
	return Vec3( a * v.x, a * v.y, a * v.z );
}

Vec3 operator*( Vec3 const v0, Vec3 const v1 )
{
	return Vec3( v0.x * v1.x, v0.y * v1.y, v0.z * v1.z );
}
Vec3 operator-( Vec3 const v )
{
	return Vec3( -v.x, -v.y, -v.z );
}

float RandFloat()
{
	return ((float)rand()) / ((float)RAND_MAX);
}

Vec3 const Vec3::ONE( 1.f, 1.f, 1.f );
Vec3 const Vec3::ZERO( 0.f, 0.f, 0.f );
Vec3 const Vec3::FORWARD( 0.f, 0.f, 1.f );
Vec3 const Vec3::BACKWARD( 0.f, 0.f, -1.f );
Vec3 const Vec3::UP( 0.f, 1.f, 0.f );
Vec3 const Vec3::DOWN( 0.f, -1.f, 0.f );
Vec3 const Vec3::LEFT( -1.f, 0.f, 0.f );
Vec3 const Vec3::RIGHT( 1.f, 0.f, 0.f );

Vec4 const Vec4::ONE( 1.f, 1.f, 1.f, 1.f );
Vec4 const Vec4::ZERO( 0.f, 0.f, 0.f, 0.f );

Quaternion const Quaternion::IDENTITY(0.f, 0.f, 0.f, 1.f);
