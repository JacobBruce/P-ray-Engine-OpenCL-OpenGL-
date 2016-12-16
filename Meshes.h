#pragma once
#include "MathExt.h"
#include "Vec3.h"
#include "Materials.h"
#include "Triangles.h"
#include "ReadWrite.h"
#include "CLTypes.h"
#include <string>
#include <fstream>
#include <assert.h>

using namespace std;

class Mesh
{
public:
	cl::Buffer* vertBuff;
	cl::Buffer* normBuff;
	cl::Buffer* triBuff;
	Vec3 boundBox[8];
	Vec3* vertices;
	Vec3* normals;
	Triangle* triangles;
	string id;
	UINT32 index;
	union {
		cl_MeshInfo info;
		struct {
			UINT32 tCount;
			UINT32 vCount;
			UINT32 nCount;
			float radius;
			Vec3 center;
		};
	};
public:
	void InitMesh()
	{
		vertBuff = nullptr;
		normBuff = nullptr;
		triBuff = nullptr;
		vertices = nullptr;
		normals = nullptr;
		triangles = nullptr;
		vCount = 0;
		tCount = 0;
		radius = 0;
		center = Vec3(0, 0, 0);
		id = "";
	}
	void FreeMesh()
	{
		if (vertices != nullptr)
		{
			delete[] vertices;
		}
		if (normals != nullptr)
		{
			delete[] normals;
		}
		if (triangles != nullptr)
		{
			delete[] triangles;
		}
		vertices = nullptr;
		normals = nullptr;
		triangles = nullptr;
	}
	Mesh()
	{
		InitMesh();
	}
	Mesh(const string filename)
	{
		InitMesh();
		Load(filename);
	}
	void LoadMesh(const string filename)
	{
		FreeMesh();
		InitMesh();
		Load(filename);
	}
	void Load(const string filename)
	{
		ifstream myfile(filename);
		string line, key, data;
		size_t bpos;
		Vec3 tempVec;
		UINT32 format, version;
		UINT32 i, index = 0;
		UINT32 vi1,vi2,vi3;
		char tChar = '\n';
		radius = 0;

		try {
			if (myfile.is_open()) {
				for (i = 0; i < 8; i++) {
					getline(myfile, line);
					bpos = line.find(" ");
					key = line.substr(0, bpos);
					data = line.substr(bpos+1);
					switch (i) {
						case 0: version = stoi(data); break;
						case 1: format = stoi(data); break;
						case 2: vCount = stoi(data); break;
						case 3: nCount = stoi(data); break;
						case 4: tCount = stoi(data); break;
						case 5: center = StrToVec3(data); break;
						case 6: radius = stof(data); break;
						case 7: id = data; break;
					}
				}

				FreeMesh();
				vertices = new Vec3[vCount];
				normals = new Vec3[nCount];
				triangles = new Triangle[tCount];

				for (i = 0; i < vCount; i++) {
					getline(myfile, line);
					vertices[i] = StrToVec3(line);
				}

				for (i = 0; i < nCount; i++) {
					getline(myfile, line);
					normals[i] = StrToVec3(line);
				}

				if (format == 0) { // text format
					for (i = 0; i < tCount; i++) {
						getline(myfile, line);
						bpos = line.find(",");
						vi1 = stoi(line.substr(0, bpos));
						line = line.substr(bpos+1);
						bpos = line.find(",");
						vi2 = stoi(line.substr(0, bpos));
						vi3 = stoi(line.substr(bpos+1));
						triangles[i].vertIndex[0] = vi1;
						triangles[i].vertIndex[1] = vi2;
						triangles[i].vertIndex[2] = vi3;
						getline(myfile, line);
						bpos = line.find(",");
						if (bpos == string::npos) { // flat
							triangles[i].type = 1;
							triangles[i].normIndex[0] = stoi(line);
						} else {
							triangles[i].type = 0;
							vi1 = stoi(line.substr(0, bpos));
							line = line.substr(bpos+1);
							bpos = line.find(",");
							vi2 = stoi(line.substr(0, bpos));
							vi3 = stoi(line.substr(bpos+1));
							triangles[i].normIndex[0] = vi1;
							triangles[i].normIndex[1] = vi2;
							triangles[i].normIndex[2] = vi3;
						}
					}
					for (i = 0; i < 8; i++) {
						getline(myfile, line);	
						boundBox[i] = StrToVec3(line);
					}
				} else if (format == 1) { // binary format
					//TODO: read binary file
				}

				myfile.close();
			} else {
				throw 130;
			}
		} catch (int ecode) {
			string emsg = ErrorCodeToStr(ecode)+filename;
			HandleFatalError(ecode, emsg);
		}
	}
	void CreateMemBuffer(cl::Context clc, cl_mem_flags flags=CL_MEM_READ_WRITE) {
		triBuff = new cl::Buffer(clc, flags, sizeof(cl_Triangle)*tCount);
		vertBuff = new cl::Buffer(clc, flags, sizeof(cl_float3)*vCount);
		normBuff = new cl::Buffer(clc, flags, sizeof(cl_float3)*nCount);
	}
	void CopyToMemBuffer(cl::CommandQueue clq, cl_bool block=CL_TRUE) {
		if (vertBuff != nullptr) {
			clq.enqueueWriteBuffer(*vertBuff, block, 0, sizeof(cl_float3)*vCount, vertices);
			clq.enqueueWriteBuffer(*triBuff, block, 0, sizeof(cl_Triangle)*tCount, triangles);
		}
		if (normBuff != nullptr) {
			clq.enqueueWriteBuffer(*normBuff, block, 0, sizeof(cl_float3)*nCount, normals);
		}
	}
	void DeleteMemBuffer() {
		if (vertBuff != nullptr) {
			delete vertBuff;
			delete triBuff;
			vertBuff = nullptr;
			triBuff = nullptr;
		}
		if (normBuff != nullptr) {
			delete normBuff;
			normBuff = nullptr;
		}	
	}
};

class MeshSet {
private:
	vector<Mesh*> meshes;
	vector<UINT32> lodMap;
public:
	UINT32 count;
public:
	MeshSet()
	{
		count = 0;
	}
	~MeshSet()
	{
		Clear();
	}
	void Clear()
	{
		if (count > 0) {
			meshes.clear();
			lodMap.clear();
			count = 0;
		}
	}
	Mesh* GetMesh(UINT32 index)
	{
		return meshes[index];
	}
	UINT32 CountLoDs(UINT32 index)
	{
		return lodMap[index];
	}
	void InsertMesh(Mesh* ot, UINT32 LoD)
	{
		meshes.push_back(ot);
		lodMap.push_back(LoD);
		count++;
	}
	void LoadMesh(const string filename)
	{
		ifstream meshfile(filename);
		string mline, mkey, mdata;
		UINT32 i, mvers, lc;
		size_t mbpos;
		string mdir, mid;

		if (meshfile.is_open()) {

			for (i = 0; i < 4; i++) {
				getline(meshfile, mline);
				mbpos = mline.find(" ");
				mkey = mline.substr(0, mbpos);
				mdata = mline.substr(mbpos+1);
				switch (i) {
					case 0: mvers = stoi(mdata); break;
					case 1: lc = stoi(mdata); break;
					case 2: mid = mdata; break;
					case 3: mdir = mdata; break;
				}
			}

			Mesh* mesh = new Mesh[lc];

			for (i = 0; i < lc; i++) {
				getline(meshfile, mline);
				mesh[i].LoadMesh(mdir+mline);
				mesh[i].id = mid+"_LoD"+IntToStr(i);
				mesh[i].index = count;
			}

			InsertMesh(mesh, lc);
			meshfile.close();
		}
	}
};