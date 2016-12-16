#pragma once
#include "cll.h"
#include "MathExt.h"

class Vec3 {
public:
	union {
		cl_float3 vector;
		struct {
			float x;
			float y;
			float z;
			float w;
		};
	};
	inline Vec3()
	{
		x = 0;
		y = 0;
		z = 0; 
	}
	inline Vec3(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	inline void set(Vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	inline void set(const float X, const float Y, const float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	inline void reset()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	inline cl_float3 toFloat3()
	{
		cl_float3 result;
		result.s[0] = x;
		result.s[1] = y;
		result.s[2] = z;
		return result;
	}
	inline bool isZero() const
	{
		if (x == 0.0f && y == 0.0f && z == 0.0f) {
			return true;
		} else {
			return false;
		}
	}
	inline float VectMag() const
	{
		return sqrt((x*x) + (y*y) + (z*z));
	}
	inline float VectSqrd(const Vec3& v) const
	{
		return pow(v.x-x, 2) + pow(v.y-y, 2) + pow(v.z-z, 2);
	}
	inline float VectDist(const Vec3& v) const
	{
		return sqrt(pow(v.x-x, 2) + pow(v.y-y, 2) + pow(v.z-z, 2));
	}
	inline float VectDot(const Vec3& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}
	inline float VectAngle(const Vec3& v) const
	{
		return acos(v.VectDot(*this));
	}
	inline Vec3 VectRefl(Vec3& norm) const
	{
		return ((norm * -2) * VectDot(norm)).VectAdd(*this);
	}
	inline Vec3 VectNorm() const
	{
		float mag = VectMag();
		return Vec3(x/mag, y/mag, z/mag);
	}
	inline Vec3 VectNeg() const
	{
		return Vec3(-x, -y, -z);
	}
	inline Vec3 VectAvg(const Vec3& v) const
	{
		return Vec3((x+v.x) / 2.0f, (y+v.y) / 2.0f, (z+v.z) / 2.0f);
	}
	inline Vec3 VectBlend(const Vec3& v, const float alpha) const
	{
		float invAlpha = 1.0f - alpha;
		return Vec3(v.x*alpha + x*invAlpha, v.y*alpha + y*invAlpha, v.z*alpha + z*invAlpha);
	}
	inline Vec3 VectCross(const Vec3& v) const
	{
		return Vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	inline Vec3 VectMult(const Vec3& v) const
	{
		return Vec3(x * v.x, y * v.y, z * v.z);
	}
	inline Vec3 VectDiv(const Vec3& v) const
	{
		return Vec3(x / v.x, y / v.y, z / v.z);
	}
	inline Vec3 VectAdd(const Vec3& v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	inline Vec3 VectSub(const Vec3& v) const
	{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}
	inline Vec3 VectMax(const float& m)
	{
		return Vec3(max(x,m), max(y,m), max(z,m));
	}
	inline Vec3 VectMin(const float& m)
	{
		return Vec3(min(x,m), min(y,m), min(z,m));
	}
	inline Vec3 VectRotX(const float& angle) const
	{
		return Vec3(x, (z*sin(angle)) + (y*cos(angle)), (z*cos(angle)) - (y*sin(angle)));
	}
	inline Vec3 VectRotY(const float& angle) const
	{
		return Vec3((z*sin(angle)) + (x*cos(angle)), y, (z*cos(angle)) - (x*sin(angle)));
	}
	inline Vec3 VectRotZ(const float& angle) const
	{
		return Vec3((x*cos(angle)) - (y*sin(angle)), (x*sin(angle)) + (y*cos(angle)), z);
	}
	inline Vec3 VectRot(const Vec3& rot) const
	{
		return VectRotY(rot.y).VectRotZ(rot.z).VectRotX(rot.x);
	}
	inline Vec3 VectRev(const Vec3& rot) const
	{
		return VectRotX(rot.x).VectRotZ(rot.z).VectRotY(rot.y);
	}
	inline Vec3 operator*(const float& scalar) const
	{
		return Vec3(x*scalar, y*scalar, z*scalar);
	}
	inline Vec3 operator/(const float& scalar) const
	{
		return Vec3(x/scalar, y/scalar, z/scalar);
	}
	inline Vec3 operator+(const float& scalar) const
	{
		return Vec3(x+scalar, y+scalar, z+scalar);
	}
	inline Vec3 operator-(const float& scalar) const
	{
		return Vec3(x-scalar, y-scalar, z-scalar);
	}
	inline bool operator==(const Vec3 &rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	inline bool operator!=(const Vec3 &rhs) const
	{
		return !(*this == rhs);
	}
	inline void operator+=(const Vec3 &rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
	}
	inline void operator-=(const Vec3 &rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
	}
	inline void operator*=(const Vec3 &rhs)
	{
		x *= rhs.x; y *= rhs.y; z *= rhs.z;
	}
	inline void operator/=(const Vec3 &rhs)
	{
		x /= rhs.x; y /= rhs.y; z /= rhs.z;
	}
};

inline string Vec3ToStr(const Vec3& v)
{
	return FltToStr(v.x) +","+ FltToStr(v.y) +","+ FltToStr(v.z);
}

inline Vec3 StrToVec3(const string line)
{
	size_t bpos;
	float xpos, ypos, zpos;
	string data;
	bpos = line.find(",");
	xpos = stof(line.substr(0, bpos));
	data = line.substr(bpos+1);
	bpos = data.find(",");
	ypos = stof(data.substr(0, bpos));
	zpos = stof(data.substr(bpos+1));
	return Vec3(xpos, ypos, zpos);
}

static Vec3 V3_X1 = Vec3( 1.0f, 0.0f, 0.0f );
static Vec3 V3_Y1 = Vec3( 0.0f, 1.0f, 0.0f );
static Vec3 V3_Z1 = Vec3( 0.0f, 0.0f, 1.0f );
static Vec3 V3_V0 = Vec3( 0.0f, 0.0f, 0.0f );
static Vec3 V3_V1 = Vec3( 1.0f, 1.0f, 1.0f );