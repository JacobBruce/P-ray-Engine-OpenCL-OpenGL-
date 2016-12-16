#pragma once
#include "Colors.h"
#include "MathExt.h"
#include "Vec3.h"
#include "ReadWrite.h"
#include "CLTypes.h"
#include <string>
#include <fstream>
#include <assert.h>
#include <gdiplus.h>

using namespace std;

struct Surface
{
	RGB32* colors;
	union {
		cl_SurfInfo info;
		struct {
			UINT32 layers;
			UINT32 height;
			UINT32 width;
			UINT32 count;
		};
	};
};

struct Surf24
{
	RGB24* colors;
	union {
		cl_SurfInfo info;
		struct {
			UINT32 layers;
			UINT32 height;
			UINT32 width;
			UINT32 count;
		};
	};
};

struct Vec3Surf
{
	Vec3* vectors;
	union {
		cl_SurfInfo info;
		struct {
			UINT32 layers;
			UINT32 height;
			UINT32 width;
			UINT32 count;
		};
	};
};

class Texture
{
public:
	cl::Buffer* texBuff;
	cl::Buffer* normBuff;
	Surface surface;
	Vec3Surf normalMap;
	bool hasNormMap;
	string id;
	UINT32 index;
public:
	Texture()
	{
		InitTexture();
	}
	~Texture()
	{
		DeleteMemBuffer();
	}
	void InitTexture() 
	{
		texBuff = nullptr;
		normBuff = nullptr;
		hasNormMap = false;
		index = 0;
		id = "";
	}
	void LoadTexture(const string filename)
	{
		surface = LoadSurface(filename);
	}
	void LoadNormalMap(const string filename)
	{
		normalMap = LoadNormMap(filename);
	}
	Surface LoadSurface(const string filename)
	{
		try {
			ifstream file(filename);
			if (file.is_open()) {
				file.close();
				Gdiplus::Bitmap bitmap( StrToWstr(filename).c_str() );
				Gdiplus::Color pixel;

				Surface surf;
				surf.width = bitmap.GetWidth();
				surf.height = bitmap.GetHeight();
				surf.count = surf.width * surf.height;
				surf.colors = new RGB32[surf.count];
				surf.layers = 1;// TODO: multi-layer surfaces

				int yy=0;

				for ( int y = surf.height-1; y >= 0; y-- ) {
					for ( UINT32 x = 0; x < surf.width; x++ )
					{
						bitmap.GetPixel( x,y,&pixel );
						surf.colors[ x + yy * surf.width ] = CREATE_ARGB32(pixel.GetR(), pixel.GetG(), pixel.GetB(), pixel.GetA());
					}
					yy++;
				}

				return surf;
			} else {
				throw 200;
			}
		} catch (int ecode) {
			string emsg = ErrorCodeToStr(ecode)+filename;
			HandleFatalError(ecode, emsg);
		}
	}
	Vec3Surf LoadNormMap(const string filename)
	{
		try {
			ifstream file(filename);
			if (file.is_open()) {
				file.close();
				Gdiplus::Bitmap bitmap( StrToWstr(filename).c_str() );
				Gdiplus::Color pixel;

				Vec3Surf surf;
				surf.width = bitmap.GetWidth();
				surf.height = bitmap.GetHeight();
				surf.count = surf.width * surf.height;
				surf.vectors = new Vec3[surf.count];
				surf.layers = 1;// TODO: multi-layer surfaces

				int yy=0;

				for( int y = surf.height-1; y >= 0; y-- ) {
					for( UINT32 x = 0; x < surf.width; x++ )
					{
						bitmap.GetPixel( x,y,&pixel );
						surf.vectors[ x + yy * surf.width ] = RGBNtoVec3(CREATE_RGB24(pixel.GetR(), pixel.GetG(), pixel.GetB()));
					}
					yy++;
				}

				return surf;
			} else {
				throw 200;
			}
		} catch (int ecode) {
			string emsg = ErrorCodeToStr(ecode)+filename;
			HandleFatalError(ecode, emsg);
		}
	}
	void CreateMemBuffer(cl::Context clc, cl_mem_flags flags=CL_MEM_READ_WRITE) {
		texBuff = new cl::Buffer(clc, flags, sizeof(cl_RGB32)*surface.count);
		if (hasNormMap) {
			normBuff = new cl::Buffer(clc, flags, sizeof(cl_float3)*normalMap.count);
		} else {
			normBuff = nullptr;
		}
	}
	void CopyToMemBuffer(cl::CommandQueue clq, cl_bool block=CL_TRUE) {
		if (texBuff != nullptr) {
			clq.enqueueWriteBuffer(*texBuff, block, 0, sizeof(cl_RGB32)*surface.count, surface.colors);
		}
		if (normBuff != nullptr) {
			clq.enqueueWriteBuffer(*normBuff, block, 0, sizeof(cl_float3)*normalMap.count, normalMap.vectors);
		}
	}
	void DeleteMemBuffer() {
		if (texBuff != nullptr) {
			delete texBuff;
			texBuff = nullptr;
		}
		if (normBuff != nullptr) {
			delete normBuff;
			normBuff = nullptr;
		}		
	}
};


class TextureSet {
private:
	vector<Texture*> textures;
	vector<UINT32> lodMap;
public:
	UINT32 count;
public:
	TextureSet()
	{
		count = 0;
	}
	~TextureSet()
	{
		Clear();
	}
	void Clear()
	{
		textures.clear();
		lodMap.clear();
		count = 0;
	}
	Texture* GetTexture(UINT32 index)
	{
		return textures[index];
	}
	UINT32 CountLoDs(UINT32 index)
	{
		return lodMap[index];
	}
	void InsertTexture(Texture* ot, UINT32 LoD)
	{
		textures.push_back(ot);
		lodMap.push_back(LoD);
		count++;
	}
	void LoadTexture(const string filename)
	{
		ifstream textfile(filename);
		string tline, tkey, tdata;
		UINT32 i, l, lc, tvers;
		size_t tbpos;
		string tdir, tid;

		if (textfile.is_open()) {

			for (i = 0; i < 4; i++) {
				getline(textfile, tline);
				tbpos = tline.find(" ");
				tkey = tline.substr(0, tbpos);
				tdata = tline.substr(tbpos+1);
				switch (i) {
					case 0: tvers = stoi(tdata); break;
					case 1: lc = stoi(tdata); break;
					case 2: tid = tdata; break;
					case 3: tdir = tdata; break;
				}
			}

			Texture* newText = new Texture[lc];

			for (l = 0; l < lc; l++) {
				newText[l].id = tid+"_LoD"+IntToStr(l);
				getline(textfile, tline);
				newText[l].LoadTexture(tdir+tline);
				getline(textfile, tline);
				if (tline != "null") {
					newText[l].LoadNormalMap(tdir+tline);
					newText[l].hasNormMap = true;
				} else {
					newText[l].hasNormMap = false;
				}
				newText[l].index = count;
			}

			InsertTexture(newText, lc);
			textfile.close();
		}
	}
};