#pragma once
#include <cstdlib>
#include <string>
#include <math.h>
#include <sstream>

using namespace std;

#define HALF_PI 1.570796326794896f
#define PI		3.141592653589793f
#define TWO_PI	6.283185307179586f

struct float2 {
	float x;
	float y;
};

struct UInt2 {
	unsigned int x;
	unsigned int y;
};

struct UInt3 {
	unsigned int x;
	unsigned int y;
	unsigned int z;
};

struct UInt4 {
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;
};

inline int round(float num)
{
	return (num > 0.0f) ? floor(num + 0.5f) : ceil(num - 0.5f);
}

inline void normAngle(float& angle)
{
	while (angle < 0.0f) { angle += TWO_PI; }
	while (angle > TWO_PI) { angle -= TWO_PI; }
}

inline void swapints(int* x1, int* x2)
{
	int temp = *x1;
	*x1 = *x2;
	*x2 = temp;
}

inline unsigned int concatints(unsigned int x, unsigned int y) {
    unsigned int pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;        
}

inline string IntToStr(int number)
{
	ostringstream s;
	s << number;
	return s.str();
}

inline string FltToStr(float number)
{
	ostringstream s;
	s << fixed << number;
	return s.str();
}

inline string DblToStr(double number)
{
	ostringstream s;
	s << number;
	return s.str();
}

inline string LngToStr(long number)
{
	ostringstream s;
	s << fixed << number;
	return s.str();
}

inline wstring StrToWstr(string str) 
{
	return wstring(str.begin(), str.end());
}

inline wstring IntToWstr(int number)
{
	return StrToWstr(IntToStr(number));
}

inline UInt2 StrToUInt2(string line)
{
	size_t bpos;
	unsigned int xpos, ypos;
	string data;
	UInt2 result;
	bpos = line.find(",");
	xpos = stoi(line.substr(0, bpos));
	ypos = stoi(line.substr(bpos+1));
	result.x = xpos;
	result.y = ypos;
	return result;
}

inline UInt3 StrToUInt3(string line)
{
	size_t bpos;
	unsigned int xpos, ypos, zpos;
	string data;
	UInt3 result;
	bpos = line.find(",");
	xpos = stoi(line.substr(0, bpos));
	data = line.substr(bpos+1);
	bpos = data.find(",");
	ypos = stoi(data.substr(0, bpos));
	zpos = stoi(data.substr(bpos+1));
	result.x = xpos;
	result.y = ypos;
	result.z = zpos;
	return result;
}


inline float2 StrToFlt2(string line)
{
	size_t bpos;
	float2 result;
	bpos = line.find(",");
	result.x = stof(line.substr(0, bpos));
	result.y = stof(line.substr(bpos+1));
	return result;
}