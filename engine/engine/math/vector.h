#pragma once

struct Vec2
{
	union
	{
		struct
		{
			float x, y;
		};
		float data[2];
	};


	Vec2()
		: x(0.f)
		, y(0.f)
	{}
	Vec2(Vec2 const& v)
		: x(v.x)
		, y(v.y)
	{}
	Vec2(float const x, float const y)
		: x(x)
		, y(y)
	{}
	Vec2(float const x)
		: x(x)
		, y(x)
	{}

	void Set(float const x, float const y)
	{
		this->x = x;
		this->y = y;
	}

	inline float Magnitude2() const
	{
		return x * x + y * y;
	}

	void Normalize()
	{
		float const radius2 = x * x + y * y;
		if (0.f < radius2)
		{
			float const invRadius = 1.f / sqrtf(radius2);
			x *= invRadius;
			y *= invRadius;
		}
	}

	Vec2 GetNormalized() const
	{
		float const radius2 = x * x + y * y;
		if (0.f < radius2)
		{
			float const invRadius = 1.f / sqrtf(radius2);
			return Vec2(x * invRadius, y * invRadius);
		}

		return Vec2(1.f, 0.f);
	}

	Vec2 operator*(Vec2 const v) const
	{
		return Vec2(x * v.x, y * v.y);
	}

	Vec2 operator/(Vec2 const v) const
	{
		return Vec2(x / v.x, y / v.y);
	}

	Vec2 operator*(float const a) const
	{
		return Vec2(x * a, y * a);
	}

	Vec2 operator+(Vec2 const v) const
	{
		return Vec2(x + v.x, y + v.y);
	}

	Vec2 operator-(Vec2 const v) const
	{
		return Vec2(x - v.x, y - v.y);
	}

	void operator=(float const a)
	{
		x = a;
		y = a;
	}

	void operator*=(float const a)
	{
		x *= a;
		y *= a;
	}

	void operator/=(float const a)
	{
		x /= a;
		y /= a;
	}

	void operator+=(Vec2 const v)
	{
		x += v.x;
		y += v.y;
	}

	void operator-=(Vec2 const v)
	{
		x -= v.x;
		y -= v.y;
	}

	bool operator==( Vec2 const other ) const
	{
		return x == other.x && y == other.y;
	}

	static Vec2 GetRandomInCircle()
	{
		float const x = RandFloat() * 2.f - 1.f;
		float const y = RandFloat() * -2.f + 1.f;

		Vec2 position(x, y);
		float const magnitude2 = position.Magnitude2();
		if (1.f < magnitude2)
		{
			position /= magnitude2;
		}

		return position;
	}

	static Vec2 GetRandomOnCircle()
	{
		float const x = RandFloat() * 2.f - 1.f;
		float const y = RandFloat() * -2.f + 1.f;

		Vec2 position(x, y);
		position.Normalize();

		return position;
	}
};

struct Vec2i
{
	union
	{
		struct
		{
			int x, y;
		};
		int data[2];
	};

	Vec2i()
		: x(0)
		, y(0)
	{}
	Vec2i(Vec2i const& v)
		: x(v.x)
		, y(v.y)
	{}
	Vec2i(int const x, int const y)
		: x(x)
		, y(y)
	{}
	void operator -=(Vec2i const& v)
	{
		x -= v.x;
		y -= v.y;
	}

	operator Vec2() const
	{
		return Vec2((float)x, (float)y);
	}
};

struct Vec3
{
	union
	{
		struct
		{
			float x, y, z;
		};
		float data[3];
	};


	static Vec3 const ONE;
	static Vec3 const ZERO;
	static Vec3 const FORWARD;
	static Vec3 const BACKWARD;
	static Vec3 const UP;
	static Vec3 const DOWN;
	static Vec3 const LEFT;
	static Vec3 const RIGHT;

	Vec3()
		: x(0.f)
		, y(0.f)
		, z(0.f)
	{}
	Vec3(Vec3 const& v)
		: x(v.x)
		, y(v.y)
		, z(v.z)
	{}
	Vec3(float const x, float const y, float const z)
		: x(x)
		, y(y)
		, z(z)
	{}

	void Set(float const x, float const y, float const z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void operator=(float const a)
	{
		x = a;
		y = a;
		z = a;
	}

	void operator/=(float const a)
	{
		x /= a;
		y /= a;
		z /= a;
	}
	void operator*=(float const a)
	{
		x *= a;
		y *= a;
		z *= a;
	}

	void operator+=(Vec3 const& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	void operator-=(Vec3 const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	void operator*=(Vec3 const& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	Vec3 operator+(Vec3 const v) const
	{
		return Vec3( x + v.x, y + v.y, z + v.z );
	}

	Vec3 operator+(float const a) const
	{
		return Vec3( x + a, y + a, z + a );
	}

	Vec3 operator-(Vec3 const v) const
	{
		return Vec3( x - v.x, y - v.y, z - v.z );
	}

	Vec3 operator-(float const a) const
	{
		return Vec3( x - a, y - a, z - a );
	}

	Vec3 operator*(float const a) const
	{
		return Vec3( x * a, y * a, z * a );
	}

	float GetMagnitude() const
	{
		float const mag = x * x + y * y + z * z;
		if (0.f < mag)
		{
			return sqrtf(mag);
		}

		return -1.f;
	}

	float GetMagnitudeSq() const
	{
		return x * x + y * y + z * z;
	}

	Vec3 GetNormalized() const
	{
		float const mag = x * x + y * y + z * z;
		if (0.f < mag)
		{
			float const invMag = 1.f / sqrtf(mag);
			return Vec3(x * invMag, y * invMag, z * invMag);
		}

		return Vec3::ZERO;
	}

	void Normalize()
	{
		float const mag = x * x + y * y + z * z;
		if (0.f < mag)
		{
			float const invMag = 1.f / sqrtf(mag);
			*this *= invMag;
			return;
		}

		*this = Vec3::ZERO;
	}

	bool operator==( Vec3 const other ) const
	{
		return x == other.x && y == other.y && z == other.z;
	}
	bool operator!=( Vec3 const other ) const
	{
		return x != other.x && y != other.y && z != other.z;
	}
	
	static float Dot( Vec3 const& v0, Vec3 const& v1 )
	{
		return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
	}
	static Vec3 Cross( Vec3 const& v0, Vec3 const& v1 )
	{
		return Vec3( v0.y * v1.z - v0.z * v1.y, v0.z * v1.x - v0.x * v1.z, v0.x * v1.y - v0.y * v1.x );
	}
};

struct Vec3i
{
	union
	{
		struct
		{
			int x, y, z;
		};
		int data[3];
	};

	Vec3i()
		: x(0)
		, y(0)
		, z(0)
	{}
	Vec3i(Vec3i const& v)
		: x(v.x)
		, y(v.y)
		, z(v.z)
	{}
	Vec3i(int const x, int const y, int const z)
		: x(x)
		, y(y)
		, z(z)
	{}

	void Set(int const _x, int const _y, int const _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	void operator -=(Vec3i const& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	operator Vec3() const
	{
		return Vec3((float)x, (float)y, (float)z);
	}
};

struct Vec4
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		float data[4];
	};


	Vec4()
		: x(0.f)
		, y(0.f)
		, z(0.f)
		, w(0.f)
	{}

	Vec4(float const x, float const y, float const z, float const w)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{}

	void Set(float const x, float const y, float const z, float const w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void operator/=(float const a)
	{
		x /= a;
		y /= a;
		z /= a;
		w /= a;
	}

	void operator=( Vec3 const& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = 1.f;
	}

	static Vec4 const ONE;
	static Vec4 const ZERO;
};

