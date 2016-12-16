#pragma once
#include "cll.h"

struct cl_RGB32
{
	cl_uchar CL_ALIGNED(4) s[4];
}; // 4 bytes

struct cl_MeshInfo
{
	cl_uint tCount;
	cl_uint vCount;
	cl_uint nCount;
	cl_float radius;
	cl_float3 center;
}; // 32 bytes

struct cl_SurfInfo
{
	cl_uint layers;
	cl_uint height;
	cl_uint width;
	cl_uint count;
}; // 16 bytes

struct cl_ObjectInfo
{
	cl_float3 center;
	cl_float3 lastPos;
	cl_float3 position;
	cl_float3 orientation;
	cl_float3 velocity;
	cl_float3 rotation;
	cl_float radius;
	cl_float radius2;
	cl_float mass;
	cl_float scale;
	cl_RGB32 color;
	cl_int type;
	cl_uint index;
	cl_uint boolBits;
}; // 128 bytes

struct cl_Material
{
	cl_float3 ambient;
	cl_float3 diffuse;
	cl_float3 specular;
	cl_float shininess;
	cl_float glossiness;
	cl_float reflectivity;
	cl_float transparency;
}; // 64 bytes

struct cl_Substance
{
	cl_uint type;
	cl_float density;
	cl_float friction;
	cl_float hardness;
	cl_float rigidity;
	cl_float strength;
	cl_float roughness;
	cl_float elasticity;
}; // 32 bytes

struct cl_Triangle
{
	cl_float2 texMap[3];
	cl_uint vertIndex[3];
	cl_uint normIndex[3];
	cl_uint texIndex;
	cl_uint matIndex;
	cl_uint subIndex;
	cl_uint type;
}; // 64 bytes

struct cl_AAInfo 
{
	cl_uint lvl;
	cl_uint dim;
	cl_float inc;
	cl_float div;
};

struct cl_RenderInfo
{	
	cl_AAInfo aa_info;
	cl_uint pixels_X;
	cl_uint t_depth;
	cl_float cam_foc;
	cl_float cam_apt;
	cl_float3 cam_pos;
	cl_float3 cam_ori;
	cl_float3 cam_fwd;
	cl_float3 cam_rgt;
	cl_float3 cam_up;
	cl_float3 bl_ray;
}; // 128 bytes

struct cl_RayIntersect
{
	union {
		struct {
			cl_float3 point;
			cl_float3 normal;
		};
		struct {
			cl_float pnt[3];
			cl_float depth;
			cl_float nrm[3];
			cl_uint matIndex;
		};
	};
}; // 32 bytes