#pragma once
#include "MathExt.h"
#include "Vec3.h"
#include "Textures.h"
#include "Meshes.h"
#include <queue>

using namespace std;

class Object {
private:
	Texture* texture;
	Mesh* mesh;
public:
	Vec3 boundBox[8];
	Vec3 bbvCache[8];
	Vec3 lastOri;
	UINT32 meshIndex, textIndex;
	UINT32 meshLod, textLod;
	UINT32 meshLods, textLods;
	UINT32 maxDist;
	float origMass;
	string name, id;
	union {
		cl_ObjectInfo info;
		struct {
			Vec3 center;
			Vec3 lastPos;
			Vec3 position;
			Vec3 orientation;
			Vec3 velocity;
			Vec3 rotation;
			float radius;
			float radius2;
			float mass;
			float scale;
			RGB32 color;
			int type;
			UINT32 index;
			struct {
				UINT32 pad : 25;
				UINT32 isVisible : 1;
				UINT32 isSolid : 1;
				UINT32 isStatic : 1;
				UINT32 isOccluder : 1;
				UINT32 isLightObj : 1;
				UINT32 showBF: 1;
				UINT32 bCached : 1;
			};
		};
	};
public:
	void FreeObject()
	{
		if (mesh != nullptr) { 
			delete[] mesh;
			mesh = nullptr;
		}
		if (texture != nullptr) { 
			delete[] texture;
			texture = nullptr;
		}
	}
	void InitObject()
	{
		center = Vec3(0, 0, 0);
		lastPos = Vec3(0, 0, 0);
		position = Vec3(0, 0, 0);
		orientation = Vec3(0, 0, 0);
		velocity = Vec3(0, 0, 0);
		rotation = Vec3(0, 0, 0);
		color = WHITE;
		mesh = nullptr;
		texture = nullptr;
		isVisible = true;
		isSolid = true;
		isStatic = true;
		isOccluder = true;
		isLightObj = false;
		bCached = false;
		showBF = false;
		maxDist = 0;
		type = 0;
		name = "";
		id = "";
		scale = 1;
		meshLods = 0;
		textLods = 0;
		meshLod = 0;
		textLod = 0;
		radius = 0.0f;
		radius2 = 0.0f;
		mass = 0.0f;
		origMass = 0.0f;
	}
	Object()
	{
		InitObject();
	}
	Mesh* GetMesh()
	{
		return &(mesh[meshLod]);
	}
	Texture* GetTexture()
	{
		return &(texture[textLod]);
	}
	Vec3Surf* GetNormMap()
	{
		return &(texture[textLod].normalMap);
	}
	void SetTexLoD(const float lod)
	{
		if (lod < textLods) {
			textLod = floor(lod);
		} else {
			textLod = textLods-1;
		}
	}
	void SetMeshLoD(const float lod)
	{
		if (lod < meshLods) {
			meshLod = floor(lod);
		} else {
			meshLod = meshLods-1;
		}
	}
	void SetLoD(const float lod)
	{
		SetTexLoD(lod);
		SetMeshLoD(lod);
	}
	void SetMesh(Mesh* pMeshes)
	{
		mesh = pMeshes;
	}
	void SetTexture(Texture* pTexts)
	{
		texture = pTexts;
	}
	void SetMeshTex(MeshSet& meshSet, TextureSet& textSet, UInt2 targ)
	{
		if (targ.x == 0 || targ.y == 0) { 
			meshLods = 0;
			textLods = 0;
			return;
		} else {
			--targ.x;
			--targ.y;
		}

		meshIndex = targ.x;
		textIndex = targ.y;

		Mesh* msh = meshSet.GetMesh(meshIndex);
		Texture* txt = textSet.GetTexture(textIndex);
		
		meshLods = meshSet.CountLoDs(meshIndex);
		textLods = textSet.CountLoDs(textIndex);

		FreeObject();
		texture = txt;
		mesh = msh;
	}
	Vec3 PointRelCenter(const Vec3& pnt)
	{
		return pnt.VectSub(center);
	}
	Vec3 PointRelOrot(const Vec3& pnt)
	{
		return pnt.VectRot(orientation);
	}
	Vec3 PointRelWorld(const Vec3& pnt)
	{
		return PointRelOrot(PointRelCenter(pnt) * scale).VectAdd(position);
	}
	void SetNameAndID(string n, string i)
	{
		name = n; id = i;
	}
	void SetRadius(const float newRad)
	{
		radius = newRad;
		radius2 = pow(radius, 2);
	}
	void SetScale(const float newScale)
	{
		scale = newScale;
		mass = pow(scale,3) * origMass;
		SetRadius(scale * mesh[0].radius);
		bCached = false;
	}
	void LoadObject(const string filename, const string objID)
	{
		ifstream objfile(filename);
		string oline, okey, odata;
		string meshfile, textfile;
		UINT32 i, version;
		size_t obpos;

		if (objfile.is_open()) {

			for (i = 0; i < 11; i++) {
				getline(objfile, oline);
				obpos = oline.find(" ");
				okey = oline.substr(0, obpos);
				odata = oline.substr(obpos+1);
				switch (i) {
					case 0: version = stoi(odata); break;
					case 1: isStatic = stoi(odata); break;
					case 2: isSolid = stoi(odata); break;
					case 3: isVisible = stoi(odata); break;
					case 4: isOccluder = stoi(odata); break;
					case 5: showBF = stoi(odata); break;
					case 6: maxDist = stoi(odata); break;
					case 7: scale = stof(odata); break;
					case 8: origMass = stof(odata); break; 
					case 9: type = stoi(odata); break;
					case 10: name = odata; break;
				}
			}

			id = objID;
			mass = origMass;

			if (type >= 0) {
				SetRadius(scale * mesh[0].radius);
				center = mesh[0].center;
				for (i=0; i<8; i++) {
					boundBox[i] = mesh[0].boundBox[i];
				}
			}
			
			objfile.close();
		}
	}
	void ApplyTexMap(const string filename, MaterialSet& mtrls) 
	{
		ifstream mapfile(filename);
		string line;
		string key, data;
		UINT32 i, ti, index;
		UINT32 version, format;
		float2* tmpVec;
		size_t bpos;

		try {
			if (mapfile.is_open()) {

				for (i = 0; i < 2; i++) {
					getline(mapfile, line);
					bpos = line.find(" ");
					key = line.substr(0, bpos);
					data = line.substr(bpos+1);
					switch (i) {
						case 0: version = stoi(data); break;
						case 1: format = stoi(data); break;
					}
				}
				if (format == 0) {
					tmpVec = new float2[3];
					for (index=0; index<meshLods; index++) {
						for (i=0; i<mesh[index].tCount; i++) {
							getline(mapfile, line);
							bpos = line.find(" ");
							key = line.substr(0, bpos);
							data = line.substr(bpos+1);
							ti = stoi(key);
							getline(mapfile, line);
							tmpVec[0] = StrToFlt2(line);
							getline(mapfile, line);
							tmpVec[1] = StrToFlt2(line);
							getline(mapfile, line);
							tmpVec[2] = StrToFlt2(line);
							mesh[index].triangles[i].UpdateTex(ti, mtrls.IndexByName(data), tmpVec);
						}
					}
					delete[] tmpVec;
				} else {
					//TODO: read binary format (fast)
				}
			} else {
				throw 150;
			}
		} catch (int ecode) {
			string emsg = ErrorCodeToStr(ecode)+filename;
			HandleFatalError(ecode, emsg);
		}
	}
	void UpdateObject(const float deltaTime) {
		lastPos = position;
		lastOri = orientation;
		if (!isStatic) {
			orientation += rotation * deltaTime;
			position += velocity * deltaTime;
			// reset vertex caches if object moved or rotated
			if (position != lastPos || orientation != lastOri) {
				bCached = false;
			}
		}
	}
};

class ObjectSet {
private:
	Object* objects[MAX_OBJECTS];
	queue<UINT32> emptySlots;
public:
	string name, id;
	UINT32 count;
public:
	ObjectSet()
	{
		count = 0;
	}
	ObjectSet(Object* obsPtr, UINT32 oCnt)
	{
		SetObjects(obsPtr, oCnt);
	}
	void ClearObjects()
	{
		for (UINT32 i = 0; i < count; i++) {
			if (objects[i] != nullptr) {
				objects[i]->FreeObject();
			}
		}
		emptySlots = queue<UINT32>();
		count = 0;
	}
	Object* ObjectByIndex(const UINT32 index)
	{
		return objects[index];
	}
	Object* ObjectByName(string objectName)
	{
		for (UINT32 i = 0; i < count; i++)
		{
			if (objects[i]->name == objectName)
			{
				return objects[i];
			}
		}
		return nullptr;
	}
	Object* ObjectByID(string objectID)
	{
		for (UINT32 i = 0; i < count; i++)
		{
			if (objects[i]->id == objectID)
			{
				return objects[i];
			}
		}
		return nullptr;
	}
	void SetObjects(Object* objArray, const UINT32 nCount)
	{
		count = nCount;
		*objects = objArray;
	}
	void InsertObject(Object* obj)
	{
		if (emptySlots.empty()) {
			objects[count] = obj;
			obj->index = count;
		} else {
			UINT32 osi = emptySlots.front();
			objects[osi] = obj;
			obj->index = osi;
			emptySlots.pop();
		}
		count++;
	}
	void RemoveObject(UINT32 index)
	{
		emptySlots.push(index);
		delete objects[index];
		objects[index] = nullptr;
		count--;
	}
	void SetNameAndID(string n, string i)
	{
		name = n;
		id = i;
	}
};

class ObjectSets {
private:
	ObjectSet* sets;
public:
	UINT32 count;
public:
	ObjectSets()
	{
		sets = nullptr;
		count = 0;
	}
	void FreeSets()
	{
		delete[] sets;
		count = 0;
	}
	void Initialize(ObjectSet* sts, UINT32 cnt)
	{
		sets = sts;
		count = cnt;
	}
	ObjectSet* GetSetByIndex(UINT32 index)
	{
		return &sets[index];
	}
	ObjectSet* GetSetByName(string setName)
	{
		for (UINT32 i = 0; i < count; i++)
		{
			if (sets[i].name == setName)
			{
				return &(sets[i]);
			}
		}
		return nullptr;
	}
	ObjectSet* GetSetByID(string setID)
	{
		for (UINT32 i = 0; i < count; i++)
		{
			if (sets[i].id == setID)
			{
				return &(sets[i]);
			}
		}
		return nullptr;
	}
};