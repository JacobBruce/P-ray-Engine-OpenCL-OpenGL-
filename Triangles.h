#pragma once
#include "MathExt.h"
#include "Vec3.h"
#include "Vec2.h"
#include <string>

class Triangle {
public:
	float2 texmap[3];
	UINT32 vertIndex[3];
	UINT32 normIndex[3];
	UINT32 texIndex;
	UINT32 matIndex;
	UINT32 subIndex;
	UINT32 type;
public:
	Triangle()
	{
		type = 1;
		texIndex = 0;
		matIndex = 0;
		subIndex = 0;
	}
	void UpdateTex(UINT32 tind, UINT32 mind, float2* txmp)
	{
		texIndex = tind;
		matIndex = mind;
		texmap[0] = txmp[0];
		texmap[1] = txmp[1];
		texmap[2] = txmp[2];
	}
};

/*inline void CopyTriVerts(Triangle& triobj, Vec3* trivec)
{
	trivec[0] = *(triobj.vertices[0]);
	trivec[1] = *(triobj.vertices[1]);
	trivec[2] = *(triobj.vertices[2]);
}

inline void CopyTriCache(Triangle& triobj, Vec3* trivec)
{
	trivec[0] = triobj.vertCache[0];
	trivec[1] = triobj.vertCache[1];
	trivec[2] = triobj.vertCache[2];
}

inline void SaveTriCache(Triangle& triobj, Vec3* trivec)
{
	triobj.vertCache[0] = trivec[0];
	triobj.vertCache[1] = trivec[1];
	triobj.vertCache[2] = trivec[2];
}

inline Vec3 interpolatePoint(Vec3* tri, Vec2& uv)
{
	return (tri[1] * uv.x).
	VectAdd(tri[2] * uv.y).
	VectAdd(tri[0] * (1.0f - uv.x - uv.y));
}

inline Vec2 interpolateTexmap(Vec2* map, Vec2& uv)
{
	return (map[1] * uv.x) + (map[2] * uv.y) +
		   (map[0] * (1.0f - uv.x - uv.y));
}

inline Vec3 interpolateNormal(Vec3** map, Vec2& uv, const bool flat)
{
	switch (flat) {
		case false: 
			return (*(map[1]) * uv.x).
			VectAdd(*(map[2]) * uv.y).
			VectAdd(*(map[0]) * (1.0f - uv.x - uv.y));
			break;
		default:
			return *(map[0]); break;
	}
}

// test primary ray against triangle for intersection
inline bool primaryRayTriIntersect(const Vec3 orig, const Vec3 dir, Vec3* tri, Vec2& uv, float& t, const bool b)
{
	Vec3 v0v1 = tri[1].VectSub(tri[0]);
	Vec3 v0v2 = tri[2].VectSub(tri[0]);
	Vec3 pvec = dir.VectCross(v0v2);
	float det = v0v1.VectDot(pvec);
	
	// if the determinant is negative the triangle is backfacing
	if (!b && det < 0.0f) { return false; }
	// if the determinant is close to 0, ray is parallel to triangle
	if (abs(det) < FLT_EPSILON) { return false; }

	float invDet = 1.0f / det;

	Vec3 tvec = orig.VectSub(tri[0]);
	uv.x = tvec.VectDot(pvec) * invDet;
	if (uv.x < 0.0f || uv.x > 1.0f) { return false; }

	Vec3 qvec = tvec.VectCross(v0v1);
	uv.y = dir.VectDot(qvec) * invDet;
	if (uv.y < 0.0f || uv.x + uv.y > 1.0f) { return false; }

	t = v0v2.VectDot(qvec) * invDet;

	return true;
}

// test secondary ray against triangle for intersection
inline bool secondRayTriIntersect(const Vec3 orig, const Vec3 dir, Vec3* tri, Vec2& uv, float& t)
{
	Vec3 v0v1 = tri[1].VectSub(tri[0]);
	Vec3 v0v2 = tri[2].VectSub(tri[0]);
	Vec3 pvec = dir.VectCross(v0v2);
	float det = v0v1.VectDot(pvec);

	if (abs(det) < FLT_EPSILON) { return false; }

	float invDet = 1.0f / det;

	Vec3 tvec = orig.VectSub(tri[0]);
	uv.x = tvec.VectDot(pvec) * invDet;
	if (uv.x < 0.0f || uv.x > 1.0f) { return false; }

	Vec3 qvec = tvec.VectCross(v0v1);
	uv.y = dir.VectDot(qvec) * invDet;
	if (uv.y < 0.0f || uv.x + uv.y > 1.0f) { return false; }

	t = v0v2.VectDot(qvec) * invDet;

	return true;
}*/