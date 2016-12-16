#pragma once 
#include <string>
#include <unordered_map>
#include "CLTypes.h"

#define CAMSPIN_SPEED	0.001
#define CAMMOVE_SPEED	1

#define OBJECT_TYPES	9
#define LIGHTS_INDEX	8
#define MAX_OBJECTS		999

#define CL_LOGGING		1
#define CL_COMPLOG		1

#define WINDOW_TITLE	"Pray Engine (OpenCL+OpenGL)"

#if defined (__APPLE__) || defined(MACOSX)
	#define CL_GL_SHARING_EXT "cl_APPLE_gl_sharing"
#else
	#define CL_GL_SHARING_EXT "cl_khr_gl_sharing"
#endif

namespace GLOBALS {

	extern std::unordered_map<std::string,std::string> config_map;

	const cl_AAInfo AA_X1  = {1,   1,   1.0,   1.0};
	const cl_AAInfo AA_X4  = {4,   2,   0.5,   0.25};
	const cl_AAInfo AA_X9  = {9,   3,   1./3,  1./6};
	const cl_AAInfo AA_X16 = {16,  4,   0.25,  0.125};
}