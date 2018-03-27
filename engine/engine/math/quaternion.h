#pragma once

struct Quaternion
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		float data[4];
	};
	Quaternion()
		: x(0.f)
		, y(0.f)
		, z(0.f)
		, w(0.f)
	{}
	Quaternion(float const _x, float const _y, float const _z, float const _w)
		: x(_x)
		, y(_y)
		, z(_z)
		, w(_w)
	{}
	void Set(float const _x, float const _y, float const _z, float const _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	Vec3 operator*(Vec3 const& vector) const
	{
		float const x2 = x * x;
		float const y2 = y * y;
		float const z2 = z * z;
		float const w2 = w * w;

		float const xz = x * z;
		float const yw = y * w;
		float const xy = x * y;
		float const zw = z * w;
		float const yz = y * z;
		float const xw = x * w;

		return Vec3
		(
			vector.x * (+x2 - y2 - z2 + w2) + 2.f * (vector.y * (xy - zw) + vector.z * (xz + yw))
			, vector.y * (-x2 + y2 - z2 + w2) + 2.f * (vector.x * (xy + zw) + vector.z * (yz - xw))
			, vector.z * (-x2 - y2 + z2 + w2) + 2.f * (vector.x * (xz - yw) + vector.y * (yz + xw))
		);
	}

	Quaternion operator*( Quaternion const& q ) const
	{
		Quaternion rQ;
		rQ.w = q.w * w - q.x * x - q.y * y - q.z * z;
		rQ.x = q.x * w + q.w * x + q.z * y - q.y * z;
		rQ.y = q.y * w - q.z * x + q.w * y + q.x * z;
		rQ.z = q.z * w + q.y * x - q.x * y + q.w * z;

		return rQ;
	}

	void operator*=( Quaternion const& q )
	{
		Quaternion rQ;
		rQ.w = q.w * w - q.x * x - q.y * y - q.z * z;
		rQ.x = q.x * w + q.w * x + q.z * y - q.y * z;
		rQ.y = q.y * w - q.z * x + q.w * y + q.x * z;
		rQ.z = q.z * w + q.y * x - q.x * y + q.w * z;

		*this = rQ;
	}

	Matrix4x4 ToMatrix4x4() const
	{
		float const x2 = x * x;
		float const y2 = y * y;
		float const z2 = z * z;
		float const w2 = w * w;

		float const xz = x * z;
		float const yw = y * w;
		float const xy = x * y;
		float const zw = z * w;
		float const yz = y * z;
		float const xw = x * w;

		return Matrix4x4
		(
			(+x2 - y2 - z2 + w2),	2.f * (xy + zw),		2.f * (xz - yw),		0.f,
			2.f * (xy - zw),		(-x2 + y2 - z2 + w2),	2.f * (yz + xw),		0.f,
			2.f * (xz + yw),		2.f * (yz - xw),		(-x2 - y2 + z2 + w2),	0.f,
			0.f,					0.f,					0.f,					1.f
		);
	}

	static Quaternion FromAngleAxis(float const angle, float const* axis)
	{
		float const halfAngle = 0.5f * angle;
		float const sinA = sinf(halfAngle);
		return Quaternion(axis[0] * sinA, axis[1] * sinA, axis[2] * sinA, cosf(halfAngle));
	}

	static Quaternion const IDENTITY;
};
