#pragma once
//struct Vector2
//{
//	float x;
//	float y;
//	Vector2  operator += (Vector2 p)
//	{
//		x += p.x;
//		y += p.y;
//		return *this;
//	}
//	Vector2  operator * (float s)
//	{
//		Vector2 ret;
//		ret.x = x * s;
//		ret.y = y * s;
//		return ret;
//	}
//	Vector2  operator / (float fValue)
//	{
//		Vector2 ret;
//		ret.x = x / fValue;
//		ret.y = y / fValue;
//		return ret;
//	}
//	Vector2  operator + (Vector2 p)
//	{
//		Vector2 ret;
//		ret.x = x + p.x;
//		ret.y = y + p.y;
//		return ret;
//	}
//	Vector2  operator - (Vector2 p)
//	{
//		Vector2 ret;
//		ret.x = x - p.x;
//		ret.y = y - p.y;
//		return ret;
//	}
//public:
//	Vector2() : x(0.f),y(0.f) {}
//	Vector2(float x, float y)
//	{
//		this->x = x;
//		this->y = y;
//	}
//};
//class Vector3
//{
//public:
//	float x;
//	float y;
//	float z;
//	Vector3  operator += (Vector3 p)
//	{
//		x += p.x;
//		y += p.y;
//		z += p.z;
//		return *this;
//	}
//	Vector3  operator * (float s)
//	{
//		Vector3 ret;
//		ret.x = x * s;
//		ret.y = y * s;
//		ret.z = z * s;
//		return ret;
//	}
//	Vector3  operator + (Vector3 p)
//	{
//		Vector3 ret;
//		ret.x = x + p.x;
//		ret.y = y + p.y;
//		ret.z = z + p.z;
//		return ret;
//	}
//	Vector3  operator - (Vector3 p)
//	{
//		Vector3 ret;
//		ret.x = x - p.x;
//		ret.y = y - p.y;
//		ret.z = z - p.z;
//		return ret;
//	}
//	Vector3  operator / (float fValue)
//	{
//		Vector3 ret;
//		ret.x = x / fValue;
//		ret.y = y / fValue;
//		ret.z = z / fValue;
//		return ret;
//	}
//public:
//	Vector3() : x(0.f),y(0.f),z(0.f){}
//	Vector3(float x, float y, float z)
//	{
//		this->x = x;
//		this->y = y;
//		this->z = z;
//	}
//};
//
//class Vector4
//{
//public:
//	float x;
//	float y;
//	float z;
//	float w;
//	Vector4() : x(0.f), y(0.f), z(0.f),w(0.f) {}
//	Vector4(float x, float y, float z, float w)
//	{
//		this->x = x;
//		this->y = y;
//		this->z = z;
//		this->w = w;
//	}
//};
#include <math.h>
#include <Windows.h>
#define TBASIS_EPSILON		((FLOAT)  0.001f) // 실수값 차이
#define TBASIS_PI			((FLOAT)  3.141592654f) // PI (라디안)
#define DegreeToRadian( degree ) ((degree) * (TBASIS_PI / 180.0f)) // 각도 -> 라디안
#define RadianToDegree( radian ) ((radian) * (180.0f / TBASIS_PI)) // 라디안 -> 각도
#define MAKECOLOR_ARGB(a, r, g, b)			(((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)
#define IS_IN_RANGE(value,r0,r1) (( ((r0) <= (value)) && ((value) <= (r1)) ) ? 1 : 0)



struct float2
{
	union
	{
		struct { float x, y; };
		float v[2];
	};
};

struct float3
{
	union
	{
		struct { float x, y, z; };
		float v[3];
	};
};
struct float4
{
	union
	{
		struct { float x, y, z, w; };
		float v[4];
	};
};
class Vector3 : public float3
{
public:
	Vector3();
	Vector3(const Vector3& v0);
	Vector3(float fX, float fY, float fZ);
	// 연산자 재정의 
	Vector3 operator + (Vector3 const& v0);
	Vector3 operator - (Vector3 const& v0);
	Vector3 operator * (float const& fScala);
	Vector3 operator + (float const& fScala);
	Vector3 operator / (float const& fScala);
	// Dot Product
	float operator | (Vector3 const& v0);
	// Cross Product
	Vector3 operator ^ (Vector3 const& v0);
	bool	operator == (Vector3 const& v0);
	// 제곱
	float LengthSquared();
	// 원점으로 부터의 거리
	float Length();
	Vector3 Normal();
	float Angle(Vector3& v0);
};
class Vector4 : public float4
{
public:
	Vector4();
	Vector4(const Vector4& v0);
	Vector4(float fX, float fY, float fZ, float fW);
};

class Vector2 : public float2
{
public:
	Vector2();
	Vector2(const Vector2& v0);
	Vector2(float fX, float fY);
public:
	Vector2 operator + (const Vector2& v);
	Vector2 operator - (const Vector2& v);
	Vector2 operator * (float value);
	Vector2 operator / (float value);
	Vector2 operator += (const Vector2& v);
	Vector2 operator -= (const Vector2& v);
	bool    operator == (const Vector2& v);
	bool    operator != (const Vector2& v);

	Vector2 Normalize();
	float	Length();
};


