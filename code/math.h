#pragma once

// TODO: Make ANSI C99 compliant

const float PI = 3.14159265358979323846f;

/*************
* Datatypes *
*************/

union Vec2
{
	float values[2];
	struct
	{
		float x;
		float y;
	};
	struct
	{
		float u;
		float v;
	};

	inline static Vec2 Right()
	{
		const Vec2 result = { 1.0f, 0.0f };
		return result;
	}

	inline static Vec2 Left()
	{
		const Vec2 result = { -1.0f, 0.0f };
		return result;
	}

	inline static Vec2 Up()
	{
		const Vec2 result = { 0.0f, 1.0f };
		return result;
	}

	inline static Vec2 Down()
	{
		const Vec2 result = { 0.0f, -1.0f };
		return result;
	}
};

union Vec3
{
	float values[3];
	struct
	{
		Vec2 xy;
		float z;
	};
	struct
	{
		float x;
		float y;
		float z;
	};
	struct
	{
		float r;
		float g;
		float b;
	};

	inline static Vec3 Right()
	{
		const Vec3 result = { 1.0f, 0.0f, 0.0f };
		return result;
	}

	inline static Vec3 Left()
	{
		const Vec3 result = { -1.0f, 0.0f, 0.0f };
		return result;
	}

	inline static Vec3 Up()
	{
		const Vec3 result = { 0.0f, 1.0f, 0.0f };
		return result;
	}

	inline static Vec3 Down()
	{
		const Vec3 result = { 0.0f, -1.0f, 0.0f };
		return result;
	}

	inline static Vec3 Back()
	{
		const Vec3 result = { 0.0f, 0.0f, 1.0f };
		return result;
	}

	inline static Vec3 Forward()
	{
		const Vec3 result = { 0.0f, 0.0f, -1.0f };
		return result;
	}

	inline static Vec3 Zero()
	{
		const Vec3 result = { 0.0f, 0.0f, 0.0f };
		return result;
	}
};

union Vec4
{
	float values[4];
	struct
	{
		Vec2 xy;
		float z;
		float w;
	};
	struct
	{
		Vec3 xyz;
		float w;
	};
	struct
	{
		float x;
		float y;
		float z;
		float w;
	};
	struct
	{
		float r;
		float g;
		float b;
		float a;
	};
	struct
	{
		Vec3 rgb;
		float a;
	};
};

union Matrix2x2
{
	float values[4];
	struct
	{
		float a11;
		float a12;
		float a21;
		float a22;
	};
};

union Matrix3x3
{
	float values[9];
	struct
	{
		float a11;
		float a12;
		float a13;
		float a21;
		float a22;
		float a23;
		float a31;
		float a32;
		float a33;
	};
};

union Matrix4x4
{
	float values[16];
	struct
	{
		float a11;
		float a12;
		float a13;
		float a14;
		float a21;
		float a22;
		float a23;
		float a24;
		float a31;
		float a32;
		float a33;
		float a34;
		float a41;
		float a42;
		float a43;
		float a44;
	};
};

union Quaternion
{
	float values[4];
	struct
	{
		float x;
		float y;
		float z;
		float w;
	};
};

/**************************
* Operations & Functions *
**************************/

// Float

float DegreesToRadians(float degrees)
{
	return degrees * PI / 180;
}

float RadiansToDegrees(float radians)
{
	return radians * 180 / PI;
}

// Vec2

inline Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
{
	const Vec2 result = { lhs.x + rhs.x, lhs.y + rhs.y };
	return result;
}

inline Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
{
	const Vec2 result = { lhs.x - rhs.x, lhs.y - rhs.y };
	return result;
}

inline Vec2 operator-(const Vec2& v)
{
	const Vec2 result = { -v.x, -v.y };
	return result;
}

inline Vec2 operator*(const Vec2& lhs, const Vec2& rhs)
{
	const Vec2 result =
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
	};

	return result;
}

inline Vec2 operator*(const Matrix2x2& lhs, const Vec2& rhs)
{
	const Vec2 result =
	{
		lhs.a11 * rhs.x + lhs.a12 * rhs.y,
		lhs.a21 * rhs.x + lhs.a22 * rhs.y
	};

	return result;
}

// Vec3

inline Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
	const Vec3 result = { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
	return result;
}

inline Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
{
	const Vec3 result = { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
	return result;
}

inline Vec3 operator-(const Vec3& v)
{
	const Vec3 result = { -v.x, -v.y, -v.z };
	return result;
}

inline Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
{
	const Vec3 result =
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z
	};

	return result;
}

inline Vec3 operator*(const Matrix3x3& lhs, const Vec3& rhs)
{
	const Vec3 result =
	{
		lhs.a11 * rhs.x + lhs.a12 * rhs.y + lhs.a13 * rhs.z,
		lhs.a21 * rhs.x + lhs.a22 * rhs.y + lhs.a23 * rhs.z,
		lhs.a31 * rhs.x + lhs.a32 * rhs.y + lhs.a33 * rhs.z
	};

	return result;
}

inline Vec3 operator*(const Matrix4x4& lhs, const Vec3& rhs)
{
	const Vec3 result =
	{
		lhs.a11 * rhs.x + lhs.a12 * rhs.y + lhs.a13 * rhs.z + lhs.a14,
		lhs.a21 * rhs.x + lhs.a22 * rhs.y + lhs.a23 * rhs.z + lhs.a24,
		lhs.a31 * rhs.x + lhs.a32 * rhs.y + lhs.a33 * rhs.z + lhs.a34
	};

	return result;
}

inline Vec3 operator*(const Vec3& lhs, const float rhs)
{
	const Vec3 result = { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
	return result;
}

inline Vec3 operator*(const float lhs, const Vec3& rhs)
{
	return rhs * lhs;
}

inline Vec3 operator/(const Vec3& lhs, const float& rhs)
{
	const Vec3 result = { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
	return result;
}

inline float Dot(const Vec3& lhs, const Vec3& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline Vec3 Cross(const Vec3& lhs, const Vec3& rhs)
{
	const Vec3 result =
	{
		(lhs.y * rhs.z) - (lhs.z * rhs.y),
		(lhs.z * rhs.x) - (lhs.x * rhs.z),
		(lhs.x * rhs.y) - (lhs.y * rhs.x)
	};

	return result;
}

inline float Length(const Vec3& v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float LengthSquared(const Vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline Vec3 Normalize(Vec3* v)
{
	const float invLength = 1.0 / Length(*v);

	v->x *= invLength;
	v->y *= invLength;
	v->z *= invLength;

	return *v;
}

inline Vec3 Normalized(const Vec3& v)
{
	const float invLength = 1.0 / Length(v);

	const Vec3 result =
	{
		v.x * invLength,
		v.y * invLength,
		v.z * invLength,
	};

	return result;
}

inline float Distance(const Vec3& v1, const Vec3& v2)
{
	return Length(v1 - v2);
}

inline float DistanceSquared(const Vec3& v1, const Vec3& v2)
{
	return LengthSquared(v1 - v2);
}

inline Vec3 Lerp(const Vec3& v1, const Vec3& v2, const float amount)
{
	const float diff = 1.0f - amount;

	const Vec3 result =
	{
		diff * v1.x + amount * v2.x,
		diff * v1.y + amount * v2.y,
		diff * v1.z + amount * v2.z
	};

	return result;
}

// Quaternion

inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
	const Quaternion result =
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z,
		lhs.w + rhs.w
	};

	return result;
}

inline Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
{
	const Quaternion result =
	{
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z,
		lhs.w - rhs.w
	};

	return result;
}

inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
	const Quaternion result =
	{
		lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
		lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
	};

	return result;
}

inline Quaternion operator*(const Quaternion& lhs, const float rhs)
{
	const Quaternion result =
	{
		lhs.x * rhs,
		lhs.y * rhs,
		lhs.z * rhs,
		lhs.w * rhs
	};

	return result;
}

inline Quaternion operator*(const float lhs, const Quaternion& rhs)
{
	const Quaternion result =
	{
		rhs.x * lhs,
		rhs.y * lhs,
		rhs.z * lhs,
		rhs.w * lhs
	};

	return result;
}

inline float Length(const Quaternion& q)
{
	return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

inline float LengthSquared(const Quaternion& q)
{
	return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

inline Quaternion Normalized(const Quaternion& q)
{
	const float invLength = 1.0 / Length(q);
	const Quaternion result =
	{
		q.x * invLength,
		q.y * invLength,
		q.z * invLength,
		q.w * invLength
	};

	return result;
}

inline Quaternion QuaternionFromAxis(const float ax, const float ay, const float az,
	const float angle)
{
	const float halfRad = angle * PI / 360;
	const float halfSin = sin(halfRad);
	const float halfCos = cos(halfRad);
	const Quaternion result =
	{
		-ax * halfSin,
		-ay * halfSin,
		-az * halfSin,
		halfCos
	};

	return Normalized(result);
}

inline Quaternion QuaternionFromAxis(const Vec3& axis, const float angle)
{
	return QuaternionFromAxis(axis.x, axis.y, axis.z, angle);
}

inline Quaternion QuaternionFromEuler(const float x, const float y, const float z)
{
	const float rx = x * PI / 360;
	const float ry = y * PI / 360;
	const float rz = z * PI / 360;

	const float sinX = -sin(rx);
	const float sinY = -sin(ry);
	const float sinZ = -sin(rz);

	const float cosX = cos(rx);
	const float cosY = cos(ry);
	const float cosZ = cos(rz);

	const float sinXsinY = sinX * sinY;
	const float sinXcosY = sinX * cosY;
	const float cosXcosY = cosX * cosY;
	const float cosXsinY = cosX * sinY;

	const Quaternion result =
	{
		cosXsinY * sinZ + sinXcosY * cosZ,
		cosXsinY * cosZ + sinXcosY * sinZ,
		cosXcosY * sinZ - sinXsinY * cosZ,
		cosXcosY * cosZ - sinXsinY * sinZ
	};

	return Normalized(result);
}

inline Quaternion QuaternionFromEuler(const Vec3& v)
{
	return QuaternionFromEuler(v.x, v.y, v.z);
}

inline Quaternion Rotated(const Quaternion& q,
	const Vec3& axis, const float angle)
{
	return Normalized(QuaternionFromAxis(axis, angle) * q);
}

inline float Dot(const Quaternion& lhs, const Quaternion& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

inline Quaternion Conjugate(const Quaternion& q)
{
	const Quaternion result = { -q.x, -q.y, -q.z, q.w };

	return result;
}

inline Quaternion Inverse(const Quaternion& q)
{
	const float invLengthSquared = 1.0 / LengthSquared(q);
	const Quaternion result =
	{
		-q.x * invLengthSquared,
		-q.y * invLengthSquared,
		-q.z * invLengthSquared,
		q.w * invLengthSquared
	};

	return result;
}


inline Vec3 Rotated(const Vec3& v, const Quaternion& q)
{
    const Vec3 u = { q.x, q.y, q.z };
	const float s = q.w;

    return 2.0f * Dot(u, v) * u
          + (s*s - Dot(u, u)) * v
          + 2.0f * s * Cross(u, v);
}

inline Vec3 Forward(const Quaternion& q)
{
	return Rotated(Vec3::Back(), q);
}

inline Vec3 Backward(const Quaternion& q)
{
	return Rotated(Vec3::Forward(), q);
}

inline Vec3 Up(const Quaternion& q)
{
	return Rotated(Vec3::Up(), q);
}

inline Vec3 Down(const Quaternion& q)
{
	return Rotated(Vec3::Down(), q);
}

inline Vec3 Left(const Quaternion& q)
{
	return Rotated(Vec3::Right(), q);
}

inline Vec3 Right(const Quaternion& q)
{
	return Rotated(Vec3::Left(), q);
}

// Matrix2x2

inline Matrix2x2 operator*(const Matrix2x2& lhs, const Matrix2x2& rhs)
{
	Matrix2x2 result;
	result.a11 = lhs.a11 * rhs.a11 + lhs.a12 * rhs.a21;
	result.a12 = lhs.a11 * rhs.a12 + lhs.a12 * rhs.a22;
	result.a21 = lhs.a21 * rhs.a11 + lhs.a22 * rhs.a21;
	result.a22 = lhs.a21 * rhs.a12 + lhs.a22 * rhs.a22;
	return result;
}

// Matrix3x3

inline Matrix3x3 operator*(const Matrix3x3& lhs, const Matrix3x3& rhs)
{
	Matrix3x3 result;
	result.a11 = lhs.a11 * rhs.a11 + lhs.a12 * rhs.a21 + lhs.a13 * rhs.a31;
	result.a12 = lhs.a11 * rhs.a12 + lhs.a12 * rhs.a22 + lhs.a13 * rhs.a32;
	result.a13 = lhs.a11 * rhs.a13 + lhs.a12 * rhs.a23 + lhs.a13 * rhs.a33;
	result.a21 = lhs.a21 * rhs.a11 + lhs.a22 * rhs.a21 + lhs.a23 * rhs.a31;
	result.a22 = lhs.a21 * rhs.a12 + lhs.a22 * rhs.a22 + lhs.a23 * rhs.a32;
	result.a23 = lhs.a21 * rhs.a13 + lhs.a22 * rhs.a23 + lhs.a23 * rhs.a33;
	result.a31 = lhs.a31 * rhs.a11 + lhs.a32 * rhs.a21 + lhs.a33 * rhs.a31;
	result.a32 = lhs.a31 * rhs.a12 + lhs.a32 * rhs.a22 + lhs.a33 * rhs.a32;
	result.a33 = lhs.a31 * rhs.a13 + lhs.a32 * rhs.a23 + lhs.a33 * rhs.a33;
	return result;
}

// Matrix4x4

inline Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	Matrix4x4 result;

	// First row
	result.a11 = lhs.a11 * rhs.a11 + lhs.a12 * rhs.a21 + lhs.a13 * rhs.a31 + lhs.a14 * rhs.a41;
	result.a12 = lhs.a11 * rhs.a12 + lhs.a12 * rhs.a22 + lhs.a13 * rhs.a32 + lhs.a14 * rhs.a42;
	result.a13 = lhs.a11 * rhs.a13 + lhs.a12 * rhs.a23 + lhs.a13 * rhs.a33 + lhs.a14 * rhs.a43;
	result.a14 = lhs.a11 * rhs.a14 + lhs.a12 * rhs.a24 + lhs.a13 * rhs.a34 + lhs.a14 * rhs.a44;

	// Second row
	result.a21 = lhs.a21 * rhs.a11 + lhs.a22 * rhs.a21 + lhs.a23 * rhs.a31 + lhs.a24 * rhs.a41;
	result.a22 = lhs.a21 * rhs.a12 + lhs.a22 * rhs.a22 + lhs.a23 * rhs.a32 + lhs.a24 * rhs.a42;
	result.a23 = lhs.a21 * rhs.a13 + lhs.a22 * rhs.a23 + lhs.a23 * rhs.a33 + lhs.a24 * rhs.a43;
	result.a24 = lhs.a21 * rhs.a14 + lhs.a22 * rhs.a24 + lhs.a23 * rhs.a34 + lhs.a24 * rhs.a44;

	// Third row
	result.a31 = lhs.a31 * rhs.a11 + lhs.a32 * rhs.a21 + lhs.a33 * rhs.a31 + lhs.a34 * rhs.a41;
	result.a32 = lhs.a31 * rhs.a12 + lhs.a32 * rhs.a22 + lhs.a33 * rhs.a32 + lhs.a34 * rhs.a42;
	result.a33 = lhs.a31 * rhs.a13 + lhs.a32 * rhs.a23 + lhs.a33 * rhs.a33 + lhs.a34 * rhs.a43;
	result.a34 = lhs.a31 * rhs.a14 + lhs.a32 * rhs.a24 + lhs.a33 * rhs.a34 + lhs.a34 * rhs.a44;

	// Fourth row
	result.a41 = lhs.a41 * rhs.a11 + lhs.a42 * rhs.a21 + lhs.a43 * rhs.a31 + lhs.a44 * rhs.a41;
	result.a42 = lhs.a41 * rhs.a12 + lhs.a42 * rhs.a22 + lhs.a43 * rhs.a32 + lhs.a44 * rhs.a42;
	result.a43 = lhs.a41 * rhs.a13 + lhs.a42 * rhs.a23 + lhs.a43 * rhs.a33 + lhs.a44 * rhs.a43;
	result.a44 = lhs.a41 * rhs.a14 + lhs.a42 * rhs.a24 + lhs.a43 * rhs.a34 + lhs.a44 * rhs.a44;

	return result;
}

inline Vec4 operator*(const Matrix4x4& lhs, const Vec4& rhs)
{
	const Vec4 result =
	{
		lhs.a11 * rhs.x + lhs.a12 * rhs.y + lhs.a13 * rhs.z + lhs.a14 * rhs.w,
		lhs.a21 * rhs.x + lhs.a22 * rhs.y + lhs.a23 * rhs.z + lhs.a24 * rhs.w,
		lhs.a31 * rhs.x + lhs.a32 * rhs.y + lhs.a33 * rhs.z + lhs.a34 * rhs.w,
		lhs.a41 * rhs.x + lhs.a42 * rhs.y + lhs.a43 * rhs.z + lhs.a44 * rhs.w
	};
	return result;
}

inline Matrix4x4 CreateMatrix4x4()
{
	const Matrix4x4 result =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	return result;
}

inline Matrix4x4 CreateMatrix4x4(const float d)
{
	const Matrix4x4 result =
	{
		d, 0, 0, 0,
		0, d, 0, 0,
		0, 0, d, 0,
		0, 0, 0, d
	};

	return result;
}

inline Matrix4x4 CreateMatrix4x4(const Quaternion& q)
{
	const float xx2 = 2.0f * q.x * q.x;
	const float xy2 = 2.0f * q.x * q.y;
	const float xz2 = 2.0f * q.x * q.z;
	const float xw2 = 2.0f * q.x * q.w;
	const float yy2 = 2.0f * q.y * q.y;
	const float yz2 = 2.0f * q.y * q.z;
	const float yw2 = 2.0f * q.y * q.w;
	const float zz2 = 2.0f * q.z * q.z;
	const float zw2 = 2.0f * q.z * q.w;

	const Matrix4x4 result =
	{
		1.0f - (yy2 + zz2), xy2 + zw2, xz2 - yw2, 0.0f,
		xy2 - zw2, 1.0f - (xx2 + zz2), yz2 + xw2, 0.0f,
		xz2 + yw2, yz2 - xw2, 1.0f - (xx2 + yy2), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

inline Matrix4x4 Transpose(const Matrix4x4& m)
{
	const Matrix4x4 result =
	{
		m.a11, m.a21, m.a31, m.a41,
		m.a12, m.a22, m.a32, m.a42,
		m.a13, m.a23, m.a33, m.a43,
		m.a14, m.a24, m.a34, m.a44
	};

	return result;
}

inline Matrix4x4 Translate(const Matrix4x4& m, const Vec3& v)
{
	const Matrix4x4 translate =
	{
		1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1
	};

	return translate * m;
}

inline Matrix4x4 Translate(const Vec3& v)
{
	const Matrix4x4 result =
	{
		1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1
	};

	return result;
}

inline Matrix4x4 Scale(const Matrix4x4& m, const Vec3& v)
{
	const Matrix4x4 scale =
	{
		v.x, 0, 0, 0,
		0, v.y, 0, 0,
		0, 0, v.z, 0,
		0, 0, 0, 1
	};

	return scale * m;
}

inline Matrix4x4 Scale(const Vec3& v)
{
	const Matrix4x4 result =
	{
		v.x, 0, 0, 0,
		0, v.y, 0, 0,
		0, 0, v.z, 0,
		0, 0, 0, 1
	};

	return result;
}

inline Matrix4x4 Mirror(const Matrix4x4& m)
{
	const Matrix4x4 scale =
	{
		-1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
	};

	return scale * m;
}

inline Matrix4x4 Mirror()
{
	const Matrix4x4 result =
	{
		-1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
	};

	return result;
}

inline Matrix4x4 Ortho(const float left, const float right,
	const float bottom, const float top,
	const float zNear, const float zFar)
{
	const float width = right - left;
	const float height = top - bottom;
	const float depth = zFar - zNear;

	const Matrix4x4 result =
	{
		2.0f/width, 0, 0, -(right + left) / width,
		0, 2.0f/height, 0, -(top + bottom) / height,
		0, 0, -2.0f/depth, -(zFar + zNear) / depth,
		0, 0, 0, 0
	};

	return result;
}

inline Matrix4x4 Perspective(const float fovy, const float aspect,
	const float zNear, const float zFar)
{
	const float angle = (fovy / 2.0f) * PI / 180.0f;
	const float yScale = 1.0f / tan(angle);
	const float xScale = yScale / aspect;
	const float depth = zFar - zNear;

	const Matrix4x4 result =
	{
		xScale, 0, 0, 0,
		0, yScale, 0, 0,
		0, 0, (zNear + zFar) / depth, -2.0f * zNear * zFar / depth,
		0, 0, 1, 0
	};

	return result;
}

inline Vec4 PerspectiveDivide(const Vec4& v)
{
	const Vec4 result = 
	{
		v.x / v.w,
		v.y / v.w,
		v.z / v.w,
		v.w		
	};
	
	return v; 
}

inline Matrix4x4 LookAt(const Vec3& eye, const Vec3& look, const Vec3& up)
{
	const Vec3 l = Normalized(look);
	const Vec3 r = Cross(look, up);
	const Vec3 u = Normalized(Cross(r, l));

	const Matrix4x4 result =
	{
		r.x, u.x, -l.x, -r.x * eye.x - u.x * eye.y + l.x * eye.z,
		r.y, u.y, -l.y, -r.y * eye.x - u.y * eye.y + l.y * eye.z,
		r.z, u.z, -l.z, -r.z * eye.x - u.z * eye.y + l.z * eye.z,
		0, 0, 0, 1,
	};

	return result;
}

inline Matrix4x4 ViewMatrix4x4(const Vec3& position, const Quaternion& orientation)
{
	const Vec3 r = Right(orientation);
	const Vec3 u = Up(orientation);
	const Vec3 f = Forward(orientation);

	const Matrix4x4 result =
	{
		r.x, r.y, r.z, -r.x * position.x - r.y * position.y - r.z * position.z,
		u.x, u.y, u.z, -u.x * position.x - u.y * position.y - u.z * position.z,
		f.x, f.y, f.z, -f.x * position.x - f.y * position.y - f.z * position.z,
		0, 0, 0, 1
	};

	return result;
}

// From normalized coordinates to screen space
inline Matrix4x4 ViewportMatrix4x4(int x, int y, int width, int height)
{
	const float halfWidth = (float) width / 2;
	const float halfHeight = (float) height / 2;
	
	const Matrix4x4 result =
	{
		halfWidth,	0,				0,		x + halfWidth,
		0,			-halfHeight,	0,		y + halfHeight,
		0, 			0, 				1,		0,
		0,			0,				0,		1
	};

	return result;
}