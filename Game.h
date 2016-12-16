#pragma once
#include "GLGraphics.h"
#include "Resource.h"
#include "CLTypes.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"
#include "FrameTimer.h"
#include "Materials.h"
#include "Objects.h"
#include "Scene.h"

class Game
{
public:
	Game(GLFWwindow* window, KeyboardServer& kServer, MouseServer& mServer);
	~Game();
	void Go();
	void ComputeStage1(Object& object);
	void ComputeStage2();
	void ComputeStage3();
private:
	void RenderScene();
	void HandleInput();
	void BeginActions();
	void ComposeFrame();
private:
	KeyboardClient kbd;
	MouseClient mouse;
	GLGraphics gfx;

	CL openCL;
	cl_int cl_error;
	cl_RenderInfo rInfo;

	cl::Buffer cl_rayBuff;
	cl::Buffer cl_pixBuff;
	cl::Buffer cl_ridBuff;
	cl::Buffer cl_cidBuff;
	cl::Buffer cl_mtrlSet;

	Scene scene;
	Camera camera;
	Font* DefFont;

	cl_float3* rayBuff;
	cl_AAInfo aaInfo;
	Vec3 bboxVert;
	Vec3 blpRay;
	Vec3 objPos;
	Vec2 coords[8];

	size_t bbsX, bbsY;
	UINT32 n,o,s,x,p;
	UINT32 pixCount, rayCount;
	UINT32 heightSpan, widthSpan;
	UINT32 heightRays, widthRays;
	UINT32 heightHalf, widthHalf;
	float maxX, minX, maxY, minY;
	float objDist, dX, dY;
	float max_distance[4];
	unsigned char trans_depth;
	unsigned char sub_rays;

	MaterialSet matSet;
	MeshSet meshSet;
	TextureSet textSet;

	float deltaTime;
	Timer deltaTimer;
};