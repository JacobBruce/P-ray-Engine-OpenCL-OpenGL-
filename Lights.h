#pragma once
#include "MathExt.h"
#include "Colors.h"
#include "Vec3.h"
#include "Objects.h"

using namespace std;

class Light {
public:
	Vec3 direction;
	Vec3 color;
	Vec3 position;
	Vec3* boundBox;
	UINT32 bulbtype;
	float range, power;
	float radius, origRad;
	Object* object;
	string id;
public:
	Light() {
		position = Vec3(0,0,0);
		direction = Vec3(0,1,0);
		color = Vec3(0.5,0.5,0.5);
		bulbtype = 0;
		range = 0.0f;
		power = 1.0f;
		radius = 1.0f;
		origRad = 1.0f;
		object = nullptr;
		boundBox = nullptr;
		id = "";
	}
	void FreeLight()
	{
		if (boundBox != nullptr) {
			delete[] boundBox;
			boundBox = nullptr;
		}
	}
	void SetPosition(const Vec3 pos)
	{
		position = pos;
		if (object != nullptr) {
			object->position = position;
		}
	}
	void SetRadius(const float rad)
	{
		radius = rad;
		if (object != nullptr) {
			object->SetRadius(radius);
		}
	}
	void UpdateObject()
	{
		if (object != nullptr) {
			object->position = position;
			if (object->radius != radius) {
				object->SetRadius(radius);
				object->scale = radius / origRad;
			}
		}
	}
	void LoadObject(const string objID, ObjectSet* objGroup=nullptr)
	{
		if (objID == "null") {
			object = new Object();
			object->name = "Invisible Light";
			object->id = id+"_object";
			object->isVisible = false;
			object->isStatic = true;
			object->isSolid = false;
		} else {
			object = objGroup->ObjectByID(objID);
		}
		if (object->boundBox != nullptr) {
			boundBox = object->boundBox;
		} else if (boundBox == nullptr) {
			boundBox = new Vec3[8];
			boundBox[0] = Vec3(radius,radius,radius);
			boundBox[1] = Vec3(-radius,radius,radius);
			boundBox[2] = Vec3(radius,radius,-radius);
			boundBox[3] = Vec3(-radius,radius,-radius);
			boundBox[4] = Vec3(radius,-radius,radius);
			boundBox[5] = Vec3(-radius,-radius,radius);
			boundBox[6] = Vec3(radius,-radius,-radius);
			boundBox[7] = Vec3(-radius,-radius,-radius);
		}
		object->isLightObj = true;
		object->color = Vec3ToColor(color);
		for (UINT32 i=0; i<8; i++) {
			object->boundBox[i] = boundBox[i];
		}
		UpdateObject();
	}
	void LoadLight(const string filename, ObjectSet& objGroup)
	{
		ifstream lightfile(filename);
		string lline, lkey, ldata;
		UINT32 i, lvers;
		size_t lbpos;
		string objID;

		if (lightfile.is_open()) {

			for (i = 0; i < 8; i++) {
				getline(lightfile, lline);
				lbpos = lline.find(" ");
				lkey = lline.substr(0, lbpos);
				ldata = lline.substr(lbpos+1);
				switch (i) {
					case 0: lvers = stoi(ldata); break;
					case 1: color = StrToVec3(ldata); break;
					case 2: bulbtype = stoi(ldata); break;
					case 3: range = stof(ldata); break;
					case 4: power = stof(ldata); break;
					case 5: radius = stof(ldata); break;
					case 6: objID = ldata; break;
					case 7: id = ldata; break;
				}
			}

			origRad = radius;
			LoadObject(objID, &objGroup);

			lightfile.close();
		}
	}
};

class LightSet {
public:
	Light* endless_lights;
	Light* falloff_lights;
	Vec3 ambLight;
	UINT32 el_count;
	UINT32 fl_count;
public:
	LightSet()
	{
		endless_lights = nullptr;
		falloff_lights = nullptr;
		el_count = 0;
		fl_count = 0;
	}
	void Initialize(Light* els, Light* fls, UINT32 ect, UINT32 fct, Vec3 alt)
	{
		endless_lights = els;
		falloff_lights = fls;
		el_count = ect;
		fl_count = fct;
		ambLight = alt;
	}
	void Clear()
	{
		if (endless_lights != nullptr) { delete[] endless_lights; }
		if (falloff_lights != nullptr) { delete[] falloff_lights; }
		endless_lights = nullptr;
		falloff_lights = nullptr;
	}
	Light* LightByID(string setID)
	{
		for (UINT32 i = 0; i < el_count; i++)
		{
			if (falloff_lights[i].id == setID)
			{
				return &(falloff_lights[i]);
			}
		}
		return nullptr;
	}
};