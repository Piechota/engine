#pragma once

struct Quaternion;

struct Matrix3x3
{
	union
	{
		struct
		{
			float m_data[9];
		};
		struct
		{
			Vec3 m_x;
			Vec3 m_y;
			Vec3 m_z;
		};
		struct
		{
			float m_a00, m_a01, m_a02;
			float m_a10, m_a11, m_a12;
			float m_a20, m_a21, m_a22;
		};
	};

	Matrix3x3()
		: m_a00(1.f), m_a01(0.f), m_a02(0.f)
		, m_a10(0.f), m_a11(1.f), m_a12(0.f)
		, m_a20(0.f), m_a21(0.f), m_a22(1.f)
	{}

	Matrix3x3(float const a00, float const a01, float const a02, float const a10, float const a11, float const a12, float const a20, float const a21, float const a22)
		: m_a00(a00), m_a01(a01), m_a02(a02)
		, m_a10(a10), m_a11(a11), m_a12(a12)
		, m_a20(a20), m_a21(a21), m_a22(a22)
	{}

	Matrix3x3(Matrix3x3 const& mat)
		: m_a00(mat.m_a00), m_a01(mat.m_a01), m_a02(mat.m_a02)
		, m_a10(mat.m_a10), m_a11(mat.m_a11), m_a12(mat.m_a12)
		, m_a20(mat.m_a20), m_a21(mat.m_a21), m_a22(mat.m_a22)
	{}

	void SetTranslateRotationSize(Vec2 const translate, Vec2 const rotation, Vec2 const size)
	{
		m_a00 = +rotation.x * size.x;		m_a01 = -rotation.y * size.y;		m_a02 = translate.x;
		m_a10 = rotation.y * size.x;		m_a11 = rotation.x * size.y;		m_a12 = translate.y;
		m_a20 = 0.f;						m_a21 = 0.f;						m_a22 = 1.f;
	}

	void SetOrthogonalMatrix(float const negative, float const positive)
	{
		float const d = 1.f / (negative - positive);
		float const a = -2.f * d;
		float const b = ( negative + positive ) * d;

		m_a00 = a;		m_a01 = 0.f;	m_a02 = b;
		m_a10 = 0.f;	m_a11 = a;		m_a12 = b;
		m_a20 = 0.f;	m_a21 = 0.f;	m_a22 = 1.f;
	}

	static Matrix3x3 GetTranslateRotationSize(Vec2 const translate, Vec2 const rotation, Vec2 const size)
	{
		Matrix3x3 mat;
		mat.m_a00 = +rotation.x * size.x;		mat.m_a01 = -rotation.y * size.y;		mat.m_a02 = translate.x;
		mat.m_a10 = rotation.y * size.x;		mat.m_a11 = rotation.x * size.y;		mat.m_a12 = translate.y;
		mat.m_a20 = 0.f;						mat.m_a21 = 0.f;						mat.m_a22 = 1.f;

		return mat;
	}

	static Matrix3x3 GetOrthogonalMatrix(float const left, float const right, float const bottom, float const top)
	{
		Matrix3x3 mat;

		float const dX = 1.f / (left - right);
		float const aX = -2.f * dX;
		float const bX = (left + right) * dX;

		float const dY = 1.f / (bottom - top);
		float const aY = -2.f * dY;
		float const bY = (bottom + top) * dY;

		mat.m_a00 = aX;		mat.m_a01 = 0.f;	mat.m_a02 = bX;
		mat.m_a10 = 0.f;	mat.m_a11 = aY;		mat.m_a12 = bY;
		mat.m_a20 = 0.f;		mat.m_a21 = 0.f;		mat.m_a22 = 1.f;

		return mat;
	}
};

struct Matrix4x4
{
	union
	{
		struct
		{
			float m_data[16];
		};
		struct
		{
			Vec4 m_x;
			Vec4 m_y;
			Vec4 m_z;
			Vec4 m_w;
		};
		struct
		{
			float m_a00, m_a01, m_a02, m_a03;
			float m_a10, m_a11, m_a12, m_a13;
			float m_a20, m_a21, m_a22, m_a23;
			float m_a30, m_a31, m_a32, m_a33;
		};
	};

	Matrix4x4()
		: m_a00(1.f), m_a01(0.f), m_a02(0.f), m_a03(0.f)
		, m_a10(0.f), m_a11(1.f), m_a12(0.f), m_a13(0.f)
		, m_a20(0.f), m_a21(0.f), m_a22(1.f), m_a23(0.f)
		, m_a30(0.f), m_a31(0.f), m_a32(0.f), m_a33(1.f)
	{}

	Matrix4x4(
		float const _a00, float const _a01, float const _a02, float const _a03,
		float const _a10, float const _a11, float const _a12, float const _a13,
		float const _a20, float const _a21, float const _a22, float const _a23,
		float const _a30, float const _a31, float const _a32, float const _a33)
		: m_a00(_a00), m_a01(_a01), m_a02(_a02), m_a03(_a03)
		, m_a10(_a10), m_a11(_a11), m_a12(_a12), m_a13(_a13)
		, m_a20(_a20), m_a21(_a21), m_a22(_a22), m_a23(_a23)
		, m_a30(_a30), m_a31(_a31), m_a32(_a32), m_a33(_a33)
	{}

	Matrix4x4(Matrix3x3 const& mat3x3)
		: m_a00(mat3x3.m_a00),	m_a01(mat3x3.m_a01),	m_a02(mat3x3.m_a02),	m_a03(0.f)
		, m_a10(mat3x3.m_a10),	m_a11(mat3x3.m_a11),	m_a12(mat3x3.m_a12),	m_a13(0.f)
		, m_a20(mat3x3.m_a20),	m_a21(mat3x3.m_a21),	m_a22(mat3x3.m_a22),	m_a23(0.f)
		, m_a30(0.f),			m_a31(0.f),				m_a32(0.f),				m_a33(1.f)
	{}

	void Set(
		float const _a00, float const _a01, float const _a02, float const _a03,
		float const _a10, float const _a11, float const _a12, float const _a13,
		float const _a20, float const _a21, float const _a22, float const _a23,
		float const _a30, float const _a31, float const _a32, float const _a33)
	{
		m_a00 = _a00; m_a01 = _a01; m_a02 = _a02; m_a03 = _a03;
		m_a10 = _a10; m_a11 = _a11; m_a12 = _a12; m_a13 = _a13;
		m_a20 = _a20; m_a21 = _a21; m_a22 = _a22; m_a23 = _a23;
		m_a30 = _a30; m_a31 = _a31; m_a32 = _a32; m_a33 = _a33;
	}

	void Transpose()
	{
		float x;
		x = m_a01;
		m_a01 = m_a10;
		m_a10 = x;

		x = m_a02;
		m_a02 = m_a20;
		m_a20 = x;

		x = m_a03;
		m_a03 = m_a30;
		m_a30 = x;

		x = m_a12;
		m_a12 = m_a21;
		m_a21 = x;

		x = m_a13;
		m_a13 = m_a31;
		m_a31 = x;

		x = m_a32;
		m_a32 = m_a23;
		m_a23 = x;
	}

	//https://stackoverflow.com/a/1148405
	bool Inverse()
	{
		float inv[16], det;
		int i;

		inv[0] = m_data[5]  * m_data[10] * m_data[15] - 
			m_data[5]  * m_data[11] * m_data[14] - 
			m_data[9]  * m_data[6]  * m_data[15] + 
			m_data[9]  * m_data[7]  * m_data[14] +
			m_data[13] * m_data[6]  * m_data[11] - 
			m_data[13] * m_data[7]  * m_data[10];

		inv[4] = -m_data[4]  * m_data[10] * m_data[15] + 
			m_data[4]  * m_data[11] * m_data[14] + 
			m_data[8]  * m_data[6]  * m_data[15] - 
			m_data[8]  * m_data[7]  * m_data[14] - 
			m_data[12] * m_data[6]  * m_data[11] + 
			m_data[12] * m_data[7]  * m_data[10];

		inv[8] = m_data[4]  * m_data[9] * m_data[15] - 
			m_data[4]  * m_data[11] * m_data[13] - 
			m_data[8]  * m_data[5] * m_data[15] + 
			m_data[8]  * m_data[7] * m_data[13] + 
			m_data[12] * m_data[5] * m_data[11] - 
			m_data[12] * m_data[7] * m_data[9];

		inv[12] = -m_data[4]  * m_data[9] * m_data[14] + 
			m_data[4]  * m_data[10] * m_data[13] +
			m_data[8]  * m_data[5] * m_data[14] - 
			m_data[8]  * m_data[6] * m_data[13] - 
			m_data[12] * m_data[5] * m_data[10] + 
			m_data[12] * m_data[6] * m_data[9];

		inv[1] = -m_data[1]  * m_data[10] * m_data[15] + 
			m_data[1]  * m_data[11] * m_data[14] + 
			m_data[9]  * m_data[2] * m_data[15] - 
			m_data[9]  * m_data[3] * m_data[14] - 
			m_data[13] * m_data[2] * m_data[11] + 
			m_data[13] * m_data[3] * m_data[10];

		inv[5] = m_data[0]  * m_data[10] * m_data[15] - 
			m_data[0]  * m_data[11] * m_data[14] - 
			m_data[8]  * m_data[2] * m_data[15] + 
			m_data[8]  * m_data[3] * m_data[14] + 
			m_data[12] * m_data[2] * m_data[11] - 
			m_data[12] * m_data[3] * m_data[10];

		inv[9] = -m_data[0]  * m_data[9] * m_data[15] + 
			m_data[0]  * m_data[11] * m_data[13] + 
			m_data[8]  * m_data[1] * m_data[15] - 
			m_data[8]  * m_data[3] * m_data[13] - 
			m_data[12] * m_data[1] * m_data[11] + 
			m_data[12] * m_data[3] * m_data[9];

		inv[13] = m_data[0]  * m_data[9] * m_data[14] - 
			m_data[0]  * m_data[10] * m_data[13] - 
			m_data[8]  * m_data[1] * m_data[14] + 
			m_data[8]  * m_data[2] * m_data[13] + 
			m_data[12] * m_data[1] * m_data[10] - 
			m_data[12] * m_data[2] * m_data[9];

		inv[2] = m_data[1]  * m_data[6] * m_data[15] - 
			m_data[1]  * m_data[7] * m_data[14] - 
			m_data[5]  * m_data[2] * m_data[15] + 
			m_data[5]  * m_data[3] * m_data[14] + 
			m_data[13] * m_data[2] * m_data[7] - 
			m_data[13] * m_data[3] * m_data[6];

		inv[6] = -m_data[0]  * m_data[6] * m_data[15] + 
			m_data[0]  * m_data[7] * m_data[14] + 
			m_data[4]  * m_data[2] * m_data[15] - 
			m_data[4]  * m_data[3] * m_data[14] - 
			m_data[12] * m_data[2] * m_data[7] + 
			m_data[12] * m_data[3] * m_data[6];

		inv[10] = m_data[0]  * m_data[5] * m_data[15] - 
			m_data[0]  * m_data[7] * m_data[13] - 
			m_data[4]  * m_data[1] * m_data[15] + 
			m_data[4]  * m_data[3] * m_data[13] + 
			m_data[12] * m_data[1] * m_data[7] - 
			m_data[12] * m_data[3] * m_data[5];

		inv[14] = -m_data[0]  * m_data[5] * m_data[14] + 
			m_data[0]  * m_data[6] * m_data[13] + 
			m_data[4]  * m_data[1] * m_data[14] - 
			m_data[4]  * m_data[2] * m_data[13] - 
			m_data[12] * m_data[1] * m_data[6] + 
			m_data[12] * m_data[2] * m_data[5];

		inv[3] = -m_data[1] * m_data[6] * m_data[11] + 
			m_data[1] * m_data[7] * m_data[10] + 
			m_data[5] * m_data[2] * m_data[11] - 
			m_data[5] * m_data[3] * m_data[10] - 
			m_data[9] * m_data[2] * m_data[7] + 
			m_data[9] * m_data[3] * m_data[6];

		inv[7] = m_data[0] * m_data[6] * m_data[11] - 
			m_data[0] * m_data[7] * m_data[10] - 
			m_data[4] * m_data[2] * m_data[11] + 
			m_data[4] * m_data[3] * m_data[10] + 
			m_data[8] * m_data[2] * m_data[7] - 
			m_data[8] * m_data[3] * m_data[6];

		inv[11] = -m_data[0] * m_data[5] * m_data[11] + 
			m_data[0] * m_data[7] * m_data[9] + 
			m_data[4] * m_data[1] * m_data[11] - 
			m_data[4] * m_data[3] * m_data[9] - 
			m_data[8] * m_data[1] * m_data[7] + 
			m_data[8] * m_data[3] * m_data[5];

		inv[15] = m_data[0] * m_data[5] * m_data[10] - 
			m_data[0] * m_data[6] * m_data[9] - 
			m_data[4] * m_data[1] * m_data[10] + 
			m_data[4] * m_data[2] * m_data[9] + 
			m_data[8] * m_data[1] * m_data[6] - 
			m_data[8] * m_data[2] * m_data[5];

		det = m_data[0] * inv[0] + m_data[1] * inv[4] + m_data[2] * inv[8] + m_data[3] * inv[12];

		if (det == 0.f)
			return false;

		det = 1.0f / det;

		for ( i = 0; i < 16; ++i )
		{
			m_data[ i ] = inv[ i ] * det;
		}
		return true;
	}

	bool Inverse( Matrix4x4& dst )
	{
		float inv[16], det;
		int i;

		inv[0] = m_data[5]  * m_data[10] * m_data[15] - 
			m_data[5]  * m_data[11] * m_data[14] - 
			m_data[9]  * m_data[6]  * m_data[15] + 
			m_data[9]  * m_data[7]  * m_data[14] +
			m_data[13] * m_data[6]  * m_data[11] - 
			m_data[13] * m_data[7]  * m_data[10];

		inv[4] = -m_data[4]  * m_data[10] * m_data[15] + 
			m_data[4]  * m_data[11] * m_data[14] + 
			m_data[8]  * m_data[6]  * m_data[15] - 
			m_data[8]  * m_data[7]  * m_data[14] - 
			m_data[12] * m_data[6]  * m_data[11] + 
			m_data[12] * m_data[7]  * m_data[10];

		inv[8] = m_data[4]  * m_data[9] * m_data[15] - 
			m_data[4]  * m_data[11] * m_data[13] - 
			m_data[8]  * m_data[5] * m_data[15] + 
			m_data[8]  * m_data[7] * m_data[13] + 
			m_data[12] * m_data[5] * m_data[11] - 
			m_data[12] * m_data[7] * m_data[9];

		inv[12] = -m_data[4]  * m_data[9] * m_data[14] + 
			m_data[4]  * m_data[10] * m_data[13] +
			m_data[8]  * m_data[5] * m_data[14] - 
			m_data[8]  * m_data[6] * m_data[13] - 
			m_data[12] * m_data[5] * m_data[10] + 
			m_data[12] * m_data[6] * m_data[9];

		inv[1] = -m_data[1]  * m_data[10] * m_data[15] + 
			m_data[1]  * m_data[11] * m_data[14] + 
			m_data[9]  * m_data[2] * m_data[15] - 
			m_data[9]  * m_data[3] * m_data[14] - 
			m_data[13] * m_data[2] * m_data[11] + 
			m_data[13] * m_data[3] * m_data[10];

		inv[5] = m_data[0]  * m_data[10] * m_data[15] - 
			m_data[0]  * m_data[11] * m_data[14] - 
			m_data[8]  * m_data[2] * m_data[15] + 
			m_data[8]  * m_data[3] * m_data[14] + 
			m_data[12] * m_data[2] * m_data[11] - 
			m_data[12] * m_data[3] * m_data[10];

		inv[9] = -m_data[0]  * m_data[9] * m_data[15] + 
			m_data[0]  * m_data[11] * m_data[13] + 
			m_data[8]  * m_data[1] * m_data[15] - 
			m_data[8]  * m_data[3] * m_data[13] - 
			m_data[12] * m_data[1] * m_data[11] + 
			m_data[12] * m_data[3] * m_data[9];

		inv[13] = m_data[0]  * m_data[9] * m_data[14] - 
			m_data[0]  * m_data[10] * m_data[13] - 
			m_data[8]  * m_data[1] * m_data[14] + 
			m_data[8]  * m_data[2] * m_data[13] + 
			m_data[12] * m_data[1] * m_data[10] - 
			m_data[12] * m_data[2] * m_data[9];

		inv[2] = m_data[1]  * m_data[6] * m_data[15] - 
			m_data[1]  * m_data[7] * m_data[14] - 
			m_data[5]  * m_data[2] * m_data[15] + 
			m_data[5]  * m_data[3] * m_data[14] + 
			m_data[13] * m_data[2] * m_data[7] - 
			m_data[13] * m_data[3] * m_data[6];

		inv[6] = -m_data[0]  * m_data[6] * m_data[15] + 
			m_data[0]  * m_data[7] * m_data[14] + 
			m_data[4]  * m_data[2] * m_data[15] - 
			m_data[4]  * m_data[3] * m_data[14] - 
			m_data[12] * m_data[2] * m_data[7] + 
			m_data[12] * m_data[3] * m_data[6];

		inv[10] = m_data[0]  * m_data[5] * m_data[15] - 
			m_data[0]  * m_data[7] * m_data[13] - 
			m_data[4]  * m_data[1] * m_data[15] + 
			m_data[4]  * m_data[3] * m_data[13] + 
			m_data[12] * m_data[1] * m_data[7] - 
			m_data[12] * m_data[3] * m_data[5];

		inv[14] = -m_data[0]  * m_data[5] * m_data[14] + 
			m_data[0]  * m_data[6] * m_data[13] + 
			m_data[4]  * m_data[1] * m_data[14] - 
			m_data[4]  * m_data[2] * m_data[13] - 
			m_data[12] * m_data[1] * m_data[6] + 
			m_data[12] * m_data[2] * m_data[5];

		inv[3] = -m_data[1] * m_data[6] * m_data[11] + 
			m_data[1] * m_data[7] * m_data[10] + 
			m_data[5] * m_data[2] * m_data[11] - 
			m_data[5] * m_data[3] * m_data[10] - 
			m_data[9] * m_data[2] * m_data[7] + 
			m_data[9] * m_data[3] * m_data[6];

		inv[7] = m_data[0] * m_data[6] * m_data[11] - 
			m_data[0] * m_data[7] * m_data[10] - 
			m_data[4] * m_data[2] * m_data[11] + 
			m_data[4] * m_data[3] * m_data[10] + 
			m_data[8] * m_data[2] * m_data[7] - 
			m_data[8] * m_data[3] * m_data[6];

		inv[11] = -m_data[0] * m_data[5] * m_data[11] + 
			m_data[0] * m_data[7] * m_data[9] + 
			m_data[4] * m_data[1] * m_data[11] - 
			m_data[4] * m_data[3] * m_data[9] - 
			m_data[8] * m_data[1] * m_data[7] + 
			m_data[8] * m_data[3] * m_data[5];

		inv[15] = m_data[0] * m_data[5] * m_data[10] - 
			m_data[0] * m_data[6] * m_data[9] - 
			m_data[4] * m_data[1] * m_data[10] + 
			m_data[4] * m_data[2] * m_data[9] + 
			m_data[8] * m_data[1] * m_data[6] - 
			m_data[8] * m_data[2] * m_data[5];

		det = m_data[0] * inv[0] + m_data[1] * inv[4] + m_data[2] * inv[8] + m_data[3] * inv[12];

		if (det == 0.f)
			return false;

		det = 1.0f / det;

		for ( i = 0; i < 16; ++i )
		{
			dst.m_data[ i ] = inv[ i ] * det;
		}
		return true;
	}

	static Matrix4x4 Projection(float const hfov, float const aspect, float const nearPlane, float const farPlane)
	{
		float const scaleXY = 1.f / tanf(hfov * MathConsts::DegToRad);
		float const scaleZ = farPlane / (farPlane - nearPlane);

		return Matrix4x4
		(
			scaleXY,	0.f,				0.f,					0.f,
			0.f,		aspect * scaleXY,	0.f,					0.f,
			0.f,		0.f,				scaleZ,					1.f,
			0.f,		0.f,				-nearPlane * scaleZ,	0.f
		);
	}

	static Matrix4x4 GetTranslateRotationSize( Vec3 const& translate, Quaternion const& rotation, Vec3 const& scale );
};

