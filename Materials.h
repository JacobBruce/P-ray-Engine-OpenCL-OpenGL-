#pragma once
#include "MathExt.h"
#include "Vec3.h"
#include <string>
#include <fstream>
#include <assert.h>

using namespace std;

struct Material {
	Vec3 ambient;
	Vec3 diffuse;
	Vec3 specular;
	float shininess;
	float glossiness;
	float reflectivity;
	float transparency;
	//float refractIndex;
};

class MaterialSet 
{
public:
	Material* materials;
	string* names;
	UINT32 count;
public:
	void InitSet()
	{
		materials = nullptr;
		names = nullptr;
		count = 0;
	}
	void FreeSet()
	{
		if (materials != nullptr) {
			delete[] materials;
		}
		if (names != nullptr) {
			delete[] names;
		}
		names = nullptr;
	}
	MaterialSet() 
	{
		InitSet();
	}
	MaterialSet(const string filename)
	{
		InitSet();
		Load(filename);
	}
	~MaterialSet() {
		FreeSet();
	}
	Material* GetByIndex(UINT32 index)
	{
		return &(materials[index]);
	}
	Material* GetByName(const string name)
	{
		for (UINT32 i = 0; i < count; i++)
		{
			if (names[i] == name)
			{
				return &(materials[i]);
			}
		}
		return nullptr;
	}
	int IndexByName(const string name)
	{
		for (int i = 0; i < count; i++)
		{
			if (names[i] == name) { return i; }
		}
		return -1;
	}
	void Load(const string filename)
	{
		ifstream myfile(filename);
		string line, key, data;
		size_t bpos;
		UINT32 i, l, version;

		try {
			if (myfile.is_open()) {

				for (i = 0; i < 2; i++) {
					getline(myfile, line);
					bpos = line.find(" ");
					key = line.substr(0, bpos);
					data = line.substr(bpos+1);
					switch (i) {
						case 0: version = stoi(data); break;
						case 1: count = stoi(data); break;
					}
				}

				FreeSet();
				materials = new Material[count];
				names = new string[count];

				for (i = 0; i < count; i++) {
					for (l = 0; l < 9; l++) {
						getline(myfile, line);
						switch (l) {
							case 0: names[i] = line; break;
							case 1: materials[i].ambient = StrToVec3(line); break;
							case 2: materials[i].diffuse = StrToVec3(line); break;
							case 3: materials[i].specular = StrToVec3(line); break;
							case 4: materials[i].shininess = stof(line); break;
							case 5: materials[i].glossiness = stof(line); break;
							case 6: materials[i].reflectivity = stof(line); break;
							case 7: materials[i].transparency = stof(line); break;
							case 8: /*materials[i].refractIndex = stof(line);*/ break;
						}
					}
				}

				myfile.close();

			} else {
				throw 110;
			}
		} catch (int ecode) {
			string emsg = ErrorCodeToStr(ecode)+filename;
			HandleFatalError(ecode, emsg);
		}
	}
};