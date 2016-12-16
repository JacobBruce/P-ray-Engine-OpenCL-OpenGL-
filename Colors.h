#pragma once
#include "Vec3.h"

struct RGB24
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

struct RGB32
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
};

struct RGB32Surf
{
	RGB32* colors;
	UINT32 count;
};

struct ColorBuffer 
{
	UInt4 avgRGBA;
	UINT32 count;
};

inline RGB32 CREATE_RGB32(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	RGB32 result;
	result.blue = b;
	result.green = g;
	result.red = r;
	result.alpha = a;
	return result;
}

inline RGB32 CREATE_XRGB32(unsigned char r, unsigned char g, unsigned char b)
{
	RGB32 result;
	result.blue = b;
	result.green = g;
	result.red = r;
	result.alpha = 255;
	return result;
}

inline RGB32 CREATE_ARGB32(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	RGB32 result;
	result.blue = b;
	result.green = g;
	result.red = r;
	result.alpha = a;
	return result;
}

inline RGB24 CREATE_RGB24(unsigned char r, unsigned char g, unsigned char b)
{
	RGB24 result;
	result.blue = b;
	result.green = g;
	result.red = r;
	return result;
}

inline Vec3 RGB24toVec3(const RGB24 src)
{
	return Vec3(src.red/(float)255, src.green/(float)255, src.blue/(float)255);
}

inline Vec3 RGBNtoVec3(const RGB24 src)
{
	return (RGB24toVec3(src) * 2) - 1;
}

inline RGB32 Vec3ToRGB32(const Vec3 v)
{
	return CREATE_XRGB32(v.x, v.y, v.z);
}

inline RGB32 Vec3ToColor(const Vec3 v)
{
	return CREATE_XRGB32(v.x*255, v.y*255, v.z*255);
}

inline float Brightness(const RGB32& c) 
{
	return (c.red + c.green + c.blue) / 3.0f;
}
	
inline RGB32 ColorScalar(const RGB32& c, float scalar) 
{
	RGB32 result;
	result.red = c.red * scalar;
	result.green = c.green * scalar;
	result.blue = c.blue * scalar;
	result.alpha = c.alpha * scalar;
	return result;
}
	
inline RGB32 ColorAdd(const RGB32& c1, const RGB32& c2) 
{
	RGB32 result;
	result.red = c1.red + c2.red;
	result.green = c1.green + c2.green;
	result.blue = c1.blue + c2.blue;
	result.alpha = c1.alpha;
	return result;
}
	
inline RGB32 ColorMultiply(const RGB32& c1, const RGB32& c2) 
{
	RGB32 result;
	result.red = c1.red * c2.red;
	result.green = c1.green * c2.green;
	result.blue = c1.blue * c2.blue;
	result.alpha = c1.alpha;
	return result;
}
	
inline RGB32 ColorAverage(const RGB32& c1, const RGB32& c2) 
{
	RGB32 result;
	result.red = (c1.red + c2.red) / 2;
	result.green = (c1.green + c2.green) / 2;
	result.blue = (c1.blue + c2.blue) / 2;
	result.alpha = c1.alpha;
	return result;
}

inline RGB32 AlphaBlend(const RGB32& c1, const RGB32& c2)
{
	RGB32 result;
	unsigned char invAlpha = (unsigned char)255 - c2.alpha;
	result.red = (c2.red * c2.alpha + c1.red * invAlpha) >> 8;
	result.green = (c2.green * c2.alpha + c1.green * invAlpha) >> 8;
	result.blue = (c2.blue * c2.alpha + c1.blue * invAlpha) >> 8;
	return result;
}

inline Vec3 ColorScalarV(const RGB32& c, float scalar) 
{
	Vec3 result;
	result.x = float(c.red) * scalar;
	result.y = float(c.green) * scalar;
	result.z = float(c.blue) * scalar;
	return result;
}
	
inline Vec3 ColorAddV(const RGB32& c, const Vec3& v) 
{
	Vec3 result;
	result.x = float(c.red) + v.x;
	result.y = float(c.green) + v.y;
	result.z = float(c.blue) + v.z;
	return result;
}

inline void ColorAddV(RGB32* c, const Vec3& v) 
{
	c->red = float(c->red) + v.x;
	c->green = float(c->green) + v.y;
	c->blue = float(c->blue) + v.z;
}
	
inline Vec3 ColorMultiplyV(const RGB32& c, const Vec3& v) 
{
	Vec3 result;
	result.x = float(c.red) * v.x;
	result.y = float(c.green) * v.y;
	result.z = float(c.blue) * v.z;
	return result;
}

inline void ColorMultiplyV(RGB32* c, const Vec3& v) 
{
	c->red = min(float(c->red) * v.x, 255);
	c->green = min(float(c->green) * v.y, 255);
	c->blue = min(float(c->blue) * v.z, 255);
}

inline RGB32 ColorClipV(const Vec3& v) {

	RGB32 result;
	result.red = max(min(round(v.x), 255), 0);
	result.green = max(min(round(v.y), 255), 0);
	result.blue = max(min(round(v.z), 255), 0);

	return result;
}

const RGB32 BLACK  = CREATE_XRGB32( 0,0,0 );
const RGB32 WHITE  = CREATE_XRGB32( 255,255,255 );
const RGB32 GRAY   = CREATE_XRGB32( 128,128,128 );
const RGB32 RED    = CREATE_XRGB32( 255,0,0 );
const RGB32 GREEN  = CREATE_XRGB32( 0,255,0 );
const RGB32 BLUE   = CREATE_XRGB32( 0,0,255 );
const RGB32 YELLOW = CREATE_XRGB32( 255,255,0 );
const RGB32 ORANGE = CREATE_XRGB32( 255,111,0 );
const RGB32 BROWN  = CREATE_XRGB32( 139,69,19 );
const RGB32 PURPLE = CREATE_XRGB32( 127,0,255 );
const RGB32 AQUA   = CREATE_XRGB32( 0,255,255 );
const RGB32 VIOLET = CREATE_XRGB32( 204,0,204 );