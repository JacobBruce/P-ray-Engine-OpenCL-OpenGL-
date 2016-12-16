#pragma once
#include "MathExt.h"
#include "Colors.h"
#include "Vec3.h"
#include "Objects.h"
#include "Lights.h"

class Camera {
public:
	Vec3 position, orientation;
	Vec3 forward, right, up;
	float foclen, aptrad;
	float sensitivity;
public:
	Camera() {
		position = Vec3(0,0,0);
		orientation = Vec3(0,0,0);
		forward = Vec3(0,0,1);
		right = Vec3(1,0,0);
		up = Vec3(0,1,0);
		foclen = 500.0f;
		aptrad = 0.0f;
	}
	Camera(Vec3 pos, Vec3 ori, Vec3 fwd, Vec3 rgt, Vec3 upv, float fl, float ar) :
		position(pos),
		orientation(ori),
		forward(fwd),
		right(rgt),
		up(upv),
		foclen(fl),
		aptrad(ar)
	{}
	Vec3 PointRelCam(const Vec3& point) const
	{
		return point.VectSub(position).VectRot(orientation);
	}
	void updateDirection()
	{
		normAngle(orientation.x);
		normAngle(orientation.y);
		normAngle(orientation.z);
		Vec3 ori = orientation.VectNeg();
		forward = V3_Z1.VectRev(ori);
		right = V3_X1.VectRev(ori);
		up = V3_Y1.VectRev(ori);
	}
};

class Scene {
public:
	LightSet lightSet;
	ObjectSets objectSets;
public:
	Scene()
	{
		lightSet = LightSet();
		objectSets = ObjectSets();
	}
	Scene(Camera sCam, LightSet sLhts, ObjectSets sObjs)
	{
		lightSet = sLhts;
		objectSets = sObjs;
	}
	void ClearScene()
	{
		lightSet.Clear();
		objectSets.FreeSets();
	}
	void LoadLevel(const string filename, Camera& camera, MaterialSet& matSet, MeshSet* meshSet, TextureSet* textSet)
	{
		ifstream levelfile(filename);
		string line, key, data;
		UINT32 i, vers;
		UINT32 mc, tc, oc;
		UINT32 elc, flc;
		Vec3 ambVec;
		size_t bpos;

		ClearScene();
		meshSet->Clear();
		textSet->Clear();

		if (levelfile.is_open()) {

			for (i = 0; i < 9; i++) {
				getline(levelfile, line);
				bpos = line.find(" ");
				key = line.substr(0, bpos);
				data = line.substr(bpos+1);
				switch (i) {
					case 0: vers = stoi(data); break;
					case 1: mc = stoi(data); break;
					case 2: tc = stoi(data); break;
					case 3: oc = stoi(data); break;
					case 4: elc = stoi(data); break;
					case 5: flc = stoi(data); break;
					case 6: camera.position = StrToVec3(data); break;
					case 7: camera.orientation = StrToVec3(data); break;
					case 8: ambVec = StrToVec3(data); break;
				}
			}

			for (i = 0; i < mc; i++) {
				getline(levelfile, line);
				meshSet->LoadMesh(line);
			}

			for (i = 0; i < tc; i++) {
				getline(levelfile, line);
				textSet->LoadTexture(line);
			}	

			ObjectSet* objects = new ObjectSet[OBJECT_TYPES];
			for (i = 0; i < oc; i++) {
				Object* obj = new Object();
				getline(levelfile, data);
				getline(levelfile, line);
				obj->SetMeshTex(*meshSet, *textSet, StrToUInt2(line));
				getline(levelfile, line);
				obj->LoadObject(line, data);
				getline(levelfile, line);
				if (line != "null") { obj->ApplyTexMap(line, matSet); }
				getline(levelfile, line);
				obj->position = StrToVec3(line);
				getline(levelfile, line);
				obj->orientation = StrToVec3(line);
				getline(levelfile, line);
				obj->rotation = StrToVec3(line);
				getline(levelfile, line);
				obj->velocity = StrToVec3(line);
				if (obj->type < 0) {
					objects[LIGHTS_INDEX].InsertObject(obj);
				} else {
					objects[obj->type].InsertObject(obj);
				}
			}

			Light* e_lights = new Light[elc];
			for (i = 0; i < elc; i++) {
				getline(levelfile, line);
				e_lights[i].LoadLight(line, objects[LIGHTS_INDEX]);
				getline(levelfile, line);
				e_lights[i].SetPosition(StrToVec3(line));
				getline(levelfile, line);
				e_lights[i].direction = StrToVec3(line);
			}

			Light* f_lights = new Light[flc];
			for (i = 0; i < flc; i++) {
				getline(levelfile, line);
				f_lights[i].LoadLight(line, objects[LIGHTS_INDEX]);
				getline(levelfile, line);
				f_lights[i].SetPosition(StrToVec3(line));
				getline(levelfile, line);
				f_lights[i].direction = StrToVec3(line);
			}

			objectSets.Initialize(objects, OBJECT_TYPES);
			lightSet.Initialize(e_lights, f_lights, elc, flc, ambVec);

			levelfile.close();
		}
	}
};