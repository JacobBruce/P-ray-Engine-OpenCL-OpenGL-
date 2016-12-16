// ------------------------------ //
// ------ CONSTANT GLOBALS ------ //
// ------------------------------ //

__constant unsigned int m_isVisible = 0x40;
__constant unsigned int m_isSolid = 0x20;
__constant unsigned int m_isStatic = 0x10;
__constant unsigned int m_isOccluder = 0x08;
__constant unsigned int m_isLightObj = 0x04;
__constant unsigned int m_showBF = 0x02;
__constant unsigned int m_bCached = 0x01;

// ------------------------------ //
// ------ TYPE DEFINITIONS ------ //
// ------------------------------ //

typedef struct {
	float3 verts[3];
} Polygon;

typedef struct {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
} RGB32;

typedef struct {
	RGB32 colors[4];
} RGB32_x4;

typedef struct {
	float3 ambient;
	float3 diffuse;
	float3 specular;
	float shininess;
	float glossiness;
	float reflectivity;
	float transparency;
} Material;

typedef struct {
	unsigned int type;
	float density;
	float friction;
	float hardness;
	float rigidity;
	float strength;
	float roughness;
	float elasticity;
} Substance;

typedef struct {
	float2 texMap[3];
	unsigned int vertIndex[3];
	unsigned int normIndex[3];
	unsigned int texIndex;
	unsigned int matIndex;
	unsigned int subIndex;
	unsigned int type;
} Triangle;

typedef struct {
	unsigned int tCount;
	unsigned int vCount;
	unsigned int nCount;
	float radius;
	float3 center;
} MeshInfo;

typedef struct {
	unsigned int layers;
	unsigned int height;
	unsigned int width;
	unsigned int count;
} SurfInfo;

typedef struct {
	float3 center;
	float3 lastPos;
	float3 position;
	float3 orientation;
	float3 velocity;
	float3 rotation;
	float radius;
	float radius2;
	float mass;
	float scale;
	RGB32 color;
	int type;
	unsigned int index;
	unsigned int boolBits;
} ObjectInfo;

typedef struct {
	unsigned int aa_lvl;
	unsigned int aa_dim;
	float aa_inc;
	float aa_div;
	unsigned int pixels_X;
	unsigned int t_depth;
	float cam_foc;
	float cam_apt;
	float3 cam_pos;
	float3 cam_ori;
	float3 cam_fwd;
	float3 cam_rgt;
	float3 cam_up;
	float3 bl_ray;
} RenderInfo;

typedef struct {
	float2 uv;
	float dist;
	bool hit;
} RTResult;

typedef struct {
	union {
		struct {
			float3 point;
			float3 normal;
		};
		struct {
			float pnt[3];
			float depth;
			float nrm[3];
			unsigned int matIndex;
		};
	};
} RayIntersect;

typedef struct {
	union {
		float4 data;
		float3 ray;
		struct {
			float direction[3];
			unsigned int intersects;
		};
	};
} PRay;

// ------------------------------ //
// ------ FLOAT3 FUNCTIONS ------ //
// ------------------------------ //

bool VectZero(const float3 v)
{
	if (v.x == 0.0f && v.y == 0.0f && v.z == 0.0f) {
		return true;
	} else {
		return false;
	}
}
float VectMag(const float3 v)
{
	return sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
float VectSqrd(const float3 v1, const float3 v2)
{
	return pow(v1.x-v2.x, 2) + pow(v1.y-v2.y, 2) + pow(v1.z-v2.z, 2);
}
float VectDist(const float3 v1, const float3 v2)
{
	return sqrt(VectSqrd(v1, v2));
}
float VectDot(const float3 v1, const float3 v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
float VectAngle(const float3 v1, const float3 v2)
{
	return acos(VectDot(v1, v2));
}
float3 VectNorm(float3 v)
{
	float mag = VectMag(v);
	return (float3)(v.x/mag, v.y/mag, v.z/mag);
}
float3 VectNeg(float3 v)
{
	return (float3)(-v.x, -v.y, -v.z);
}
float3 VectAvg(const float3 v1, const float3 v2)
{
	return (float3)((v1.x+v2.x) / 2.0f, (v1.y+v2.y) / 2.0f, (v1.z+v2.z) / 2.0f);
}
float3 VectBlend(const float3 v1, const float3 v2, const float alpha)
{
	float invAlpha = 1.0f - alpha;
	return (float3)(v1.x*alpha + v2.x*invAlpha, v1.y*alpha + v2.y*invAlpha, v1.z*alpha + v2.z*invAlpha);
}
float3 VectCross(const float3 v1, const float3 v2)
{
	return (float3)(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}
float3 VectMax(const float3 v, const float m)
{
	return (float3)(max(v.x,m), max(v.y,m), max(v.z,m));
}
float3 VectMin(const float3 v, const float m)
{
	return (float3)(min(v.x,m), min(v.y,m), min(v.z,m));
}
float3 VectRefl(const float3 v, const float3 norm)
{
	return ((norm * -2) * VectDot(norm, v)) + v;
}
float3 VectRotX(const float3 v, const float angle)
{
	return (float3)(v.x, (v.z*sin(angle)) + (v.y*cos(angle)), (v.z*cos(angle)) - (v.y*sin(angle)));
}
float3 VectRotY(const float3 v, const float angle)
{
	return (float3)((v.z*sin(angle)) + (v.x*cos(angle)), v.y, (v.z*cos(angle)) - (v.x*sin(angle)));
}
float3 VectRotZ(const float3 v, const float angle)
{
	return (float3)((v.x*cos(angle)) - (v.y*sin(angle)), (v.x*sin(angle)) + (v.y*cos(angle)), v.z);
}
float3 VectRot(const float3 v, const float3 rot)
{
	float3 result = VectRotY(v, rot.y);
	result = VectRotZ(result, rot.z);
	return VectRotX(result, rot.x);
}
float3 VectRev(const float3 v, const float3 rot)
{
	float3 result = VectRotY(v, rot.x);
	result = VectRotZ(result, rot.z);
	return VectRotX(result, rot.y);
}

// ------------------------------ //
// ------ COLOUR FUNCTIONS ------ //
// ------------------------------ //

RGB32 CREATE_RGB32(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	RGB32 result;
	result.blue = b;
	result.green = g;
	result.red = r;
	result.alpha = a;
	return result;
}

RGB32 CREATE_XRGB32(unsigned char r, unsigned char g, unsigned char b)
{
	RGB32 result;
	result.blue = b;
	result.green = g;
	result.red = r;
	result.alpha = 255;
	return result;
}

RGB32 AlphaBlend(const RGB32 c1, const RGB32 c2)
{
	RGB32 result;
	unsigned char invAlpha = (unsigned char)255 - c2.alpha;
	result.red = (c2.red * c2.alpha + c1.red * invAlpha) >> 8;
	result.green = (c2.green * c2.alpha + c1.green * invAlpha) >> 8;
	result.blue = (c2.blue * c2.alpha + c1.blue * invAlpha) >> 8;
	return result;
}

float4 VectToColor(const float3 v)
{
	return (float4)(v / 255, 1.0f);
}

// ------------------------------ //
// ------- RAY FUNCTIONS -------- //
// ------------------------------ //

RTResult primaryRayTriIntersect(const float3 orig, const float3 dir, const Polygon tri, const bool b)
{
	float3 v0v1 = tri.verts[1] - tri.verts[0];
	float3 v0v2 = tri.verts[2] - tri.verts[0];
	float3 pvec = VectCross(dir, v0v2);
	float det = VectDot(v0v1, pvec);
	
	RTResult result;
	result.hit = false;

	if (!b && det < 0.0f) { return result; }
	if (fabs(det) < FLT_EPSILON) { return result; }

	float invDet = 1.0f / det;
	
	float3 tvec = orig - tri.verts[0];
	result.uv.x = VectDot(tvec, pvec) * invDet;
	if (result.uv.x < 0.0f || result.uv.x > 1.0f) { return result; }

	float3 qvec = VectCross(tvec, v0v1);
	result.uv.y = VectDot(dir, qvec) * invDet;
	if (result.uv.y < 0.0f || result.uv.x + result.uv.y > 1.0f) { return result; }

	result.dist = VectDot(v0v2, qvec) * invDet;

	result.hit = true;
	return result;
}

RTResult secondRayTriIntersect(const float3 orig, const float3 dir, const Polygon tri)
{
	float3 v0v1 = tri.verts[1] - tri.verts[0];
	float3 v0v2 = tri.verts[2] - tri.verts[0];
	float3 pvec = VectCross(dir, v0v2);
	float det = VectDot(v0v1, pvec);
	
	RTResult result;
	result.hit = false;

	if (fabs(det) < FLT_EPSILON) { return result; }

	float invDet = 1.0f / det;

	float3 tvec = orig - tri.verts[0];
	result.uv.x = VectDot(tvec, pvec) * invDet;
	if (result.uv.x < 0.0f || result.uv.x > 1.0f) { return result; }

	float3 qvec = VectCross(tvec, v0v1);
	result.uv.y = VectDot(dir, qvec) * invDet;
	if (result.uv.y < 0.0f || result.uv.x + result.uv.y > 1.0f) { return result; }

	result.dist = VectDot(v0v2, qvec) * invDet;

	result.hit = true;
	return result;
}

float raySphereIntersect(const float3 orig, const float3 dir, const float3 pos, const float rad2)
{
	float t0, t1;
	float3 L = pos - orig;
	float tca = VectDot(L, dir);
	
	if (tca < 0.0f) { return 0.0f; }
	float d2 = VectDot(L, L) - tca * tca;
	if (d2 > rad2) { return 0.0f; }
	float thc = sqrt(rad2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	if (t0 > t1) {
		float temp = t0;
		t0 = t1;
		t1 = temp;
	}

	if (t0 < 0.0f) {
		if (t1 < 0.0f) { return 0.0f; }
	}

	return t0;
}

// ------------------------------ //
// ------ INTERP FUNCTIONS ------ //
// ------------------------------ //

float2 InterpolateTexmap(const float2 tm0, const float2 tm1, const float2 tm2, const float2 uv)
{
	return (tm1 * uv.x) + (tm2 * uv.y) +
		   (tm0 * (1.0f - uv.x - uv.y));
}

float3 InterpolatePoly(Polygon poly, const float2 uv)
{
	return (poly.verts[1] * uv.x) + (poly.verts[2] * uv.y) +
		   (poly.verts[0] * (1.0f - uv.x - uv.y));
}

float3 InterpolateNorm(__global float3* verts, const unsigned int* vi, const float2 uv, const unsigned char type)
{
	if (type != 1) {
		return (verts[vi[1]] * uv.x) + (verts[vi[2]] * uv.y) +
			   (verts[vi[0]] * (1.0f - uv.x - uv.y));
	} else {
		return verts[vi[0]];
	}
}

RGB32 InterpolateSurf(__global RGB32* surf, __global Material* mats, 
					 const Triangle tri, const float2 uv, const SurfInfo sinfo)
{
	float2 texCoords = InterpolateTexmap(tri.texMap[0], tri.texMap[1], tri.texMap[2], uv);
	unsigned int iX = texCoords.x * (sinfo.width-1);
	unsigned int iY = texCoords.y * (sinfo.height-1);
	RGB32 pntColor = surf[iX + (iY * sinfo.width)];
	pntColor.alpha *= mats[tri.matIndex].transparency;
	return pntColor;
}

Polygon TriRelWorld(__global float3* verts, const Triangle tri, const ObjectInfo object_info)
{
	Polygon poly;
	poly.verts[0] = VectRot((verts[tri.vertIndex[0]] - object_info.center) * 
					object_info.scale, object_info.orientation) + object_info.position;
	poly.verts[1] = VectRot((verts[tri.vertIndex[1]] - object_info.center) * 
					object_info.scale, object_info.orientation) + object_info.position;
	poly.verts[2] = VectRot((verts[tri.vertIndex[2]] - object_info.center) * 
					object_info.scale, object_info.orientation) + object_info.position;
	return poly;
}

// ------------------------------ //
// ------ KERNEL FUNCTIONS ------ //
// ------------------------------ //

__kernel void ComputeStage0x1(__global PRay* ray_buffer, const RenderInfo render_info)
{
    unsigned int pix_X = get_global_id(0);
	unsigned int pix_Y = get_global_id(1);
	unsigned int pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	PRay primaryRay;
	
	primaryRay.ray = render_info.bl_ray + (render_info.cam_rgt * pix_X);
	primaryRay.ray = VectNorm(primaryRay.ray + (render_info.cam_up * pix_Y));
	primaryRay.intersects = 0;
	ray_buffer[pix_index] = primaryRay;
}

__kernel void ComputeStage0xN(__global PRay* ray_buffer, const RenderInfo render_info)
{
    unsigned int pix_X = get_global_id(0);
	unsigned int pix_Y = get_global_id(1);
    float max_X = (float)pix_X + 1.0f;
	float max_Y = (float)pix_Y + 1.0f;
	unsigned int pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	unsigned int ray_index = pix_index * render_info.aa_lvl;
	PRay primaryRay;
	
	for (float yr=pix_Y+render_info.aa_div; yr < max_Y; yr += render_info.aa_inc) {
		for (float xr=pix_X+render_info.aa_div; xr < max_X; xr += render_info.aa_inc) {
		
			primaryRay.ray = render_info.bl_ray + (render_info.cam_rgt * xr);
			primaryRay.ray = VectNorm(primaryRay.ray + (render_info.cam_up * yr));
			primaryRay.intersects = 0;
			ray_buffer[ray_index] = primaryRay;
			ray_index++;
		}
	}
}

__kernel void ComputeStage1S(__global PRay* ray_buffer, __global RayIntersect* rid_buffer,
__global RGB32* cid_buffer, const ObjectInfo object_info, const RenderInfo render_info)
{
    unsigned int pix_X = get_global_id(0);
	unsigned int pix_Y = get_global_id(1);
	unsigned int pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	unsigned int ray_index = pix_index * render_info.aa_lvl;
	
	for (unsigned int r=render_info.aa_lvl; r-- > 0; ray_index++) {
	
		unsigned int rid_index = ray_index * render_info.t_depth;
		PRay primRay = ray_buffer[ray_index];
		unsigned char ric = primRay.intersects;
		
		float sDist = raySphereIntersect(render_info.cam_pos,
					  primRay.ray, object_info.position, object_info.radius2);
		
		if (sDist > 0.0f) {
		
			if (ric >= render_info.t_depth) {
				if (sDist > rid_buffer[rid_index+ric-1].depth) {
					continue;
				}			
			}	
			
			unsigned char d = 0;
			
			for (unsigned int ii=rid_index; d<=ric; ii++) {
				if (d == ric) {
				
					if (ric == render_info.t_depth) { break; }

					RayIntersect tmpRid;
					float3 pntVect = render_info.cam_pos + (primRay.ray * sDist);
					float3 nrmVect = VectNorm(pntVect - object_info.position);
					tmpRid.point = pntVect;
					tmpRid.normal = nrmVect;
					tmpRid.depth = sDist;
					tmpRid.matIndex = 0;
					cid_buffer[ii] = object_info.color;
					rid_buffer[ii] = tmpRid;
					ray_buffer[ray_index].intersects++;
					break;
				}
				
				if (sDist < rid_buffer[ii].depth) {	
				
					while (d < ric) {
						rid_buffer[rid_index+d+1] = rid_buffer[rid_index+d];
						d++;
					}
					
					RayIntersect tmpRid;
					float3 pntVect = render_info.cam_pos + (primRay.ray * sDist);
					float3 nrmVect = VectNorm(pntVect - object_info.position);
					tmpRid.point = pntVect;
					tmpRid.normal = nrmVect;
					tmpRid.depth = sDist;
					tmpRid.matIndex = 0;
					cid_buffer[ii] = object_info.color;
					rid_buffer[ii] = tmpRid;
					ray_buffer[ray_index].intersects++;
					break;
				}
				d++;
			}
		}
	}
}

__kernel void ComputeStage1T(__global PRay* ray_buffer, __global RayIntersect* rid_buffer,
__global RGB32* cid_buffer, __global Material* mat_set, __global float3* verts, __global Triangle* mesh,
__global RGB32* texture, __global float3* norm_map, const ObjectInfo object_info,
const MeshInfo mesh_info, const SurfInfo surf_info, const RenderInfo render_info)
{
	__local bool insideBS;
	__local float sDist;
    unsigned int pix_X = get_global_id(0);
	unsigned int pix_Y = get_global_id(1);
	unsigned int pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	unsigned int ray_index = pix_index * render_info.aa_lvl;
	sDist = VectSqrd(object_info.position, render_info.cam_pos);
	insideBS = sDist < object_info.radius2;
	
	for (unsigned int r=render_info.aa_lvl; r-- > 0; ray_index++) {

		unsigned int rid_index = ray_index * render_info.t_depth;	
		PRay primRay = ray_buffer[ray_index];
		unsigned char ric = primRay.intersects;

		if (!insideBS) {
			sDist = raySphereIntersect(render_info.cam_pos, primRay.ray, 
					object_info.position, object_info.radius2);
			
			if (sDist > 0.0f) {
				if (ric == render_info.t_depth) {
					if (sDist > rid_buffer[rid_index+ric-1].depth) {
						continue;
					}			
				}
			} else {
				continue;
			}
		}
		
		for (unsigned int ti=0; ti<mesh_info.tCount; ti++) {
		
			Triangle tri = mesh[ti];
			Polygon poly = TriRelWorld(verts, tri, object_info);
			
			RTResult rtr = primaryRayTriIntersect(render_info.cam_pos, 
						   primRay.ray, poly, object_info.boolBits & m_showBF);
			
			if (rtr.hit) {
			
				float3 pntVect = InterpolatePoly(poly, rtr.uv);
				float3 socPnt = VectRot(pntVect - render_info.cam_pos, render_info.cam_ori);
	
				if (socPnt.z > 0.0f) {
				
					unsigned char d = 0;
					
					for (unsigned int ii=rid_index; d<=ric; ii++) {
						if (d == ric) {
						
							if (d > 0 && cid_buffer[ii-1].alpha == 255) { break; }
							if (ric == render_info.t_depth) { break; }
							
							RayIntersect tmpRid;	
							RGB32 pntColor = InterpolateSurf(texture, mat_set, tri, rtr.uv, surf_info);
							float3 nrmVect = InterpolateNorm(verts, &(tri.normIndex[0]), rtr.uv, tri.type);
							tmpRid.point = pntVect;
							tmpRid.normal = nrmVect;
							tmpRid.depth = rtr.dist;
							tmpRid.matIndex = tri.matIndex;
							cid_buffer[ii] = pntColor;	
							rid_buffer[ii] = tmpRid;
							
							if (pntColor.alpha == 255) {
								ric = d+1;
							} else {
								ric++;
							}
							break;
						}
						
						if (rtr.dist < rid_buffer[ii].depth) {
						
							if (d > 0 && cid_buffer[ii-1].alpha == 255) { break; }
							if (ric == render_info.t_depth) { ric--; }
							
							for (unsigned int l = 0; d++ < ric; l++) {
								rid_buffer[rid_index+ric-l-1] = rid_buffer[rid_index+ric-l];
								cid_buffer[rid_index+ric-l-1] = cid_buffer[rid_index+ric-l];
							}
							
							RayIntersect tmpRid;
							RGB32 pntColor = InterpolateSurf(texture, mat_set, tri, rtr.uv, surf_info);
							float3 nrmVect = InterpolateNorm(verts, &(tri.normIndex[0]), rtr.uv, tri.type);
							tmpRid.point = pntVect;
							tmpRid.normal = nrmVect;
							tmpRid.depth = rtr.dist;
							tmpRid.matIndex = tri.matIndex;
							cid_buffer[ii] = pntColor;
							rid_buffer[ii] = tmpRid;
							
							if (pntColor.alpha == 255) {
								ric = d+1;
							} else {
								ric++;
							}
							break;
						}
						d++;
					}
					if (primRay.intersects != ric) {
						ray_buffer[ray_index].intersects = ric;
					}
				}
			}
		}
	}
}

__kernel void ComputeStage2x4(__global PRay* ray_buffer, __global RGB32_x4* cid_buffer,
write_only image2d_t pix_buffer, const RenderInfo render_info)
{
    unsigned int pix_X = get_global_id(0);
	unsigned int pix_Y = get_global_id(1);
	unsigned int pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	unsigned int ray_index = pix_index * render_info.aa_lvl;
	float3 sumColor = (float3)(0.0f,0.0f,0.0f);
	unsigned char ric;
	RGB32 itpColor;
		
	for (unsigned int r=render_info.aa_lvl; r-- > 0; ray_index++) {
	
		ric = ray_buffer[ray_index].intersects;
		RGB32_x4 ray_cid = cid_buffer[ray_index];
		
		if (ric > 1) {
		
			itpColor = ray_cid.colors[ric-1];
			
			for (int ii=ric-2; ii > -1; ii--) {
				itpColor = AlphaBlend(itpColor, ray_cid.colors[ii]);
			}

			sumColor.x += itpColor.red;
			sumColor.y += itpColor.green;
			sumColor.z += itpColor.blue;
			
		} else if (ric == 1) {
		
			itpColor = ray_cid.colors[0];

			sumColor.x += itpColor.red;
			sumColor.y += itpColor.green;
			sumColor.z += itpColor.blue;
		}
	}
	
	write_imagef(pix_buffer, (int2)(pix_X, pix_Y), VectToColor(sumColor * render_info.aa_div));
}

__kernel void ComputeStage2x1(__global PRay* ray_buffer, __global RGB32* cid_buffer,
write_only image2d_t pix_buffer, const RenderInfo render_info)
{
    unsigned int pix_X = get_global_id(0);
	unsigned int pix_Y = get_global_id(1);
	unsigned int pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	unsigned int ray_index = pix_index * render_info.aa_lvl;
	float3 sumColor = (float3)(0.0f,0.0f,0.0f);
	RGB32 itpColor;
		
	for (unsigned int r=render_info.aa_lvl; r-- > 0; ray_index++) {
	
		itpColor = cid_buffer[ray_index];

		sumColor.x += itpColor.red;
		sumColor.y += itpColor.green;
		sumColor.z += itpColor.blue;
	}
	
	write_imagef(pix_buffer, (int2)(pix_X, pix_Y), VectToColor(sumColor * render_info.aa_div));
}
