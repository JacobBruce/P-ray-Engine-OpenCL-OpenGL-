#include "Game.h"
#include <time.h>
#include <cstdlib>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

Game::Game(GLFWwindow* window, KeyboardServer& kServer, MouseServer& mServer)
:
	kbd( kServer ),
	mouse( mServer )
{
	// ensure the compiler is playing nice
	assert(sizeof(RGB32) == sizeof(cl_RGB32) && sizeof(RGB32) == 4);
	assert(sizeof(Vec3) == sizeof(cl_float3) && sizeof(Vec3) == 16);
	assert(sizeof(Material) == sizeof(cl_Material) && sizeof(Material) == 64);
	assert(sizeof(Triangle) == sizeof(cl_Triangle) && sizeof(Triangle) == 64);
	assert(sizeof(cl_SurfInfo) == 16);
	assert(sizeof(cl_MeshInfo) == 32);
	assert(sizeof(cl_Substance) == 32); // TODO: substance stuff
	assert(sizeof(cl_RayIntersect) == 32);
	assert(sizeof(cl_ObjectInfo) == 128);
	assert(sizeof(cl_RenderInfo) == 128);

	// use settings previously loaded from file
	max_distance[0] = stof(GLOBALS::config_map["MAX_DISTANCE1"]);
	max_distance[1] = stof(GLOBALS::config_map["MAX_DISTANCE2"]);
	max_distance[2] = stof(GLOBALS::config_map["MAX_DISTANCE3"]);
	max_distance[3] = stof(GLOBALS::config_map["MAX_DISTANCE4"]);

	trans_depth = stoi(GLOBALS::config_map["TRANS_DEPTH"]);
	sub_rays = stoi(GLOBALS::config_map["AA_SUB_RAYS"]);

	switch (sub_rays) {
		case 1: aaInfo = GLOBALS::AA_X1; break;
		case 4: aaInfo = GLOBALS::AA_X4; break;
		case 9: aaInfo = GLOBALS::AA_X9; break;
		case 16: aaInfo = GLOBALS::AA_X16; break;
		default:
			string emsg = "Invalid AA level detected: "+GLOBALS::config_map["AA_SUB_RAYS"];
			HandleFatalError(1, emsg);
			break;
	}

	// Initialize OpenCL
	openCL.Initialize(sub_rays, trans_depth);

	// Initialize graphics manager
	gfx.Initialize(window, openCL.context());

	// load default font
	DefFont = new Font("Data\\fonts\\GenericFont.bmp", "generic", 16, 16, 16, BLACK);

	// load material properties library
	matSet.Load("Data\\materials.mpl");

	// set camera sensitivity based on settings
	camera.sensitivity = stof(GLOBALS::config_map["MOUSE_SENSI"]);

	// load objects, lights, etc, from level layout file
	scene.LoadLevel("Data\\levels\\level_0.llf", camera, matSet, &meshSet, &textSet);

	// calc useful screen info
	widthHalf = gfx.windowWidth / 2;
	heightHalf = gfx.windowHeight / 2;
	widthRays = gfx.windowWidth * aaInfo.lvl;
	heightRays = gfx.windowHeight * aaInfo.lvl;
	heightSpan = gfx.windowHeight - 1;
	widthSpan = gfx.windowWidth - 1;
	pixCount = gfx.windowWidth * gfx.windowHeight;
	rayCount = pixCount * aaInfo.lvl;

	// set dimensions of local work groups
	for (p=64; p>0; p--) {
		n = p * 64;
		if (n <= openCL.max_wg_size && (pixCount % n == 0)) {
			openCL.local_range = cl::NDRange(n, 1);
			break;
		}
	}

	if (p == 0) {
		string emsg = "Incompatible resolution detected: "+IntToStr(gfx.windowWidth)+"x"+IntToStr(gfx.windowHeight);
		HandleFatalError(2, emsg);
	}

	// setup RenderInfo structure
	//rInfo.pix_count = pixCount;
	//rInfo.ray_count = rayCount;
	rInfo.pixels_X = gfx.windowWidth;
	//rInfo.pixels_Y = windHeight;
	//rInfo.rays_X = widthRays;
	//rInfo.rays_Y = heightRays;
	rInfo.aa_info = aaInfo;
	rInfo.t_depth = trans_depth;
	//rInfo.d_time = 0.0f;

	// allocate and copy to memory on GPU for material buffer
	cl_mtrlSet = cl::Buffer(openCL.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_Material)*matSet.count, matSet.materials);

	// allocate memory on GPU for pixel buffer
	cl_pixBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_RGB32)*pixCount);

	// allocate memory on GPU for primary ray buffer
	cl_rayBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_float4)*rayCount);

	// allocate memory on GPU for ray color buffer
	cl_cidBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_RGB32)*rayCount*trans_depth);

	// allocate memory on GPU for intersection buffer
	cl_ridBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_RayIntersect)*rayCount*trans_depth);

	// copy mesh set to GPU memory
	for (UINT32 mi=0; mi<meshSet.count; mi++) {
		Mesh* mesh = meshSet.GetMesh(mi);
		for (UINT32 ml=0; ml<meshSet.CountLoDs(mi); ml++) {
			mesh[ml].CreateMemBuffer(openCL.context);
			mesh[ml].CopyToMemBuffer(openCL.queue);
		}

	}
	// copy texture set to GPU memory
	for (UINT32 ti=0; ti<textSet.count; ti++) {
		Texture* texture = textSet.GetTexture(ti);
		for (UINT32 tl=0; tl<textSet.CountLoDs(ti); tl++) {
			texture[tl].CreateMemBuffer(openCL.context);
			texture[tl].CopyToMemBuffer(openCL.queue);
		}
	}
}


Game::~Game()
{
	// remove meshes from GPU memory
	for (UINT32 mi=0; mi<meshSet.count; mi++) {
		Mesh* mesh = meshSet.GetMesh(mi);
		for (UINT32 ml=0; ml<meshSet.CountLoDs(mi); ml++) {
			mesh[ml].DeleteMemBuffer();
		}
	}
	// remove textures from GPU memory
	for (UINT32 ti=0; ti<textSet.count; ti++) {
		Texture* texture = textSet.GetTexture(ti);
		for (UINT32 tl=0; tl<textSet.CountLoDs(ti); tl++) {
			texture[tl].DeleteMemBuffer();
		}
	}
}

void Game::Go()
{
	deltaTimer.StopWatch();
	deltaTime = deltaTimer.GetTimeMilli();
	deltaTimer.StartWatch();
	//gfx.BeginFrame();
	ComposeFrame();
	gfx.DisplayFrame();
}

void Game::HandleInput()
{
	// keys for left and right tilt
	if (kbd.KeyIsPressed(GLFW_KEY_E)) {
		camera.orientation.z -= CAMSPIN_SPEED * deltaTime;
	} else if (kbd.KeyIsPressed(GLFW_KEY_Q)) {
		camera.orientation.z += CAMSPIN_SPEED * deltaTime;
	}

	KeyEvent ke = kbd.ReadKey();
	if (ke.IsPress()) {
		switch (ke.GetCode()) {
		case GLFW_KEY_NUM_LOCK:
			gfx.ToggleCursorLock();
			break;
		default: break;
		}
	}

	// handle mouse events
	MouseEvent me = mouse.ReadMouse();
	switch (me.GetType()) {
	case MouseEvent::Move:
		if (gfx.cursorLocked) {
			dX = me.GetX() * camera.sensitivity * deltaTime;
			dY = me.GetY() * camera.sensitivity * deltaTime;
			camera.orientation.x += dY;
			camera.orientation.y -= dX;
			glfwSetCursorPos(gfx.window, 0.0, 0.0);
		}
		break;
	case MouseEvent::WheelUp:
		camera.foclen += 10;
		break;
	case MouseEvent::WheelDown:
		camera.foclen -= 10;
		break;
	}

	// update camera direction
	camera.updateDirection();

	// keys for global ambient light intensity
	if (kbd.KeyIsPressed(GLFW_KEY_UP)) {
		scene.lightSet.ambLight + 0.02f;
		scene.lightSet.ambLight.VectMin(1.0f);
	} else if (kbd.KeyIsPressed(GLFW_KEY_DOWN)) {
		scene.lightSet.ambLight - 0.02f;
		scene.lightSet.ambLight.VectMax(0.0f);
	}

	// keys for up and down movement
	if (kbd.KeyIsPressed(GLFW_KEY_PAGE_UP)) {
		camera.position += camera.up * (CAMMOVE_SPEED * deltaTime);
	} else if (kbd.KeyIsPressed(GLFW_KEY_PAGE_DOWN)) {
		camera.position -= camera.up * (CAMMOVE_SPEED * deltaTime);
	}

	// keys for forward and backward movement
	if (kbd.KeyIsPressed(GLFW_KEY_W)) {
		camera.position += camera.forward * (CAMMOVE_SPEED * deltaTime);
	} else if (kbd.KeyIsPressed(GLFW_KEY_S)) {
		camera.position -= camera.forward * (CAMMOVE_SPEED * deltaTime);
	}

	// keys for right and left movement
	if (kbd.KeyIsPressed(GLFW_KEY_D)) {
		camera.position += camera.right * (CAMMOVE_SPEED * deltaTime);
	} else if (kbd.KeyIsPressed(GLFW_KEY_A)) {
		camera.position -= camera.right * (CAMMOVE_SPEED * deltaTime);
	}	
}

void Game::BeginActions()
{
	// calculate bottom left position of virtual screen
	blpRay = (camera.forward * camera.foclen).
			 VectSub(camera.right * widthHalf).
			 VectSub(camera.up * heightHalf);

	// save camera info to RenderInfo structure
	rInfo.cam_foc = camera.foclen;
	rInfo.cam_apt = camera.aptrad;
	rInfo.cam_pos = camera.position.toFloat3();
	rInfo.cam_ori = camera.orientation.toFloat3();
	rInfo.cam_fwd = camera.forward.toFloat3();
	rInfo.cam_rgt = camera.right.toFloat3();
	rInfo.cam_up = camera.up.toFloat3();
	rInfo.bl_ray = blpRay.toFloat3();
	//rInfo.d_time = deltaTime;

	// compute primary rays
	openCL.CR_Kernel.setArg(0, cl_rayBuff);
	openCL.CR_Kernel.setArg(1, rInfo);
	openCL.RunKernel0(gfx.windowWidth, gfx.windowHeight);
	openCL.queue.finish();
}

void Game::ComputeStage1(Object& object)
{
	// apply motion if non-static object
	object.UpdateObject(deltaTime);

	// check if object is visible
	if (!object.isVisible) { return; }

	// get object position relative to cam
	objPos = camera.PointRelCam(object.position);

	// get minimum distance to object bounding sphere
	objDist = max(camera.position.VectDist(object.position) - object.radius, 0.0f);

	// skip objects beyond max view distance
	switch (object.maxDist) {
		case 0: break;
		case 1: if (objDist > max_distance[0]) { return; } break;
		case 2: if (objDist > max_distance[1]) { return; } break;
		case 3: if (objDist > max_distance[2]) { return; } break;
		case 4: if (objDist > max_distance[3]) { return; } break;
	}

	// TODO: deal with lights

	// check if object is behind camera
	if (objPos.z+object.radius <= 0.0f) { return; }

	// works best when mesh & texture detail halves with each level of detail
	object.SetLoD(max(sqrt(objPos.VectMag()/camera.foclen)-1.0f, 0.0f));

	maxX = maxY = 0.0f;
	minX = minY = FLT_MAX;

	// check if we should update object bounding box cache
	if (!object.bCached) {
		for (p=0; p<8; p++) {
			object.bbvCache[p] = object.PointRelWorld(object.boundBox[p]);
			object.bCached = true;
		}
	}

	// transform points on bounding box to screen coordinates
	for (p=0; p<8; p++) {

		// get corner of bounding box relative to cam
		bboxVert = camera.PointRelCam(object.bbvCache[p]);

		// map corner vertex onto screen coords
		if (bboxVert.z < 0.0f) {
			// fix coordinates for points behind cam
			coords[p].x = (bboxVert.x < 0.0f) ? -1.0f : gfx.windowWidth;
			coords[p].y = (bboxVert.y < 0.0f) ? -1.0f : gfx.windowHeight;
		} else {
			coords[p].x = widthHalf + (bboxVert.x / bboxVert.z) * camera.foclen;
			coords[p].y = heightHalf + (bboxVert.y / bboxVert.z) * camera.foclen;
		}

		// get top left and bottom right coordinates
		minX = min(coords[p].x, minX);
		maxX = max(coords[p].x, maxX);
		minY = min(coords[p].y, minY);
		maxY = max(coords[p].y, maxY);
	}

	// cull objects not in field of view
	if (((minX < 0.0f && maxX < 0.0f) || (minX > widthSpan && maxX > widthSpan))
	|| ((minY < 0.0f && maxY < 0.0f) || (minY > widthSpan && maxY > widthSpan))) 
	{ return; }

	// clip coordinates if outside screen
	minX = min(max(minX, 0.0f), widthSpan);
	minY = min(max(minY, 0.0f), heightSpan);
	maxX = min(maxX, widthSpan);
	maxY = min(maxY, heightSpan);

	// calc rays needed for each axis of 2D BB
	bbsX = maxX - minX + 1;
	bbsY = maxY - minY + 1;

	// check if object is analytical sphere
	if (object.type == -1) {

		/*CS_Kernel.setArg(0, cl_rayBuff);
		openCL.CS_Kernel.setArg(1, cl_ridBuff);
		openCL.CS_Kernel.setArg(2, cl_cidBuff);
		openCL.CS_Kernel.setArg(3, object.info);
		openCL.CS_Kernel.setArg(4, rInfo);

		openCL.queue.enqueueNDRangeKernel(CS_Kernel, cl::NDRange((size_t)minX, (size_t)minY), cl::NDRange(bbsX, bbsY), local_range);
		openCL.queue.finish();*/

	} else {

		Mesh* pMesh = object.GetMesh();
		Texture* pTex = object.GetTexture();

		cl_MeshInfo meshInfo = pMesh->info;
		cl_SurfInfo surfInfo = pTex->surface.info;

		openCL.CT_Kernel.setArg(0, cl_rayBuff);
		openCL.CT_Kernel.setArg(1, cl_ridBuff);
		openCL.CT_Kernel.setArg(2, cl_cidBuff);
		openCL.CT_Kernel.setArg(3, cl_mtrlSet);

		openCL.CT_Kernel.setArg(4, *(pMesh->vertBuff));
		openCL.CT_Kernel.setArg(5, *(pMesh->triBuff));
		openCL.CT_Kernel.setArg(6, *(pTex->texBuff));

		if (pTex->hasNormMap) {
			openCL.CT_Kernel.setArg(7, *(pTex->normBuff));
		} else {
			openCL.CT_Kernel.setArg(7, NULL);
		}

		openCL.CT_Kernel.setArg(8, object.info);
		openCL.CT_Kernel.setArg(9, meshInfo);
		openCL.CT_Kernel.setArg(10, surfInfo);
		openCL.CT_Kernel.setArg(11, rInfo);

		// send rays through area covered by 2D bounding box
		openCL.RunKernel1(minX, minY, bbsX, bbsY);
		openCL.queue.finish();
	}
}

void Game::ComputeStage2()
{
	// compute final pixel colors
	openCL.CL_Kernel.setArg(0, cl_rayBuff);
	openCL.CL_Kernel.setArg(1, cl_cidBuff);
	openCL.CL_Kernel.setArg(2, gfx.gl_backBuff);
	openCL.CL_Kernel.setArg(3, rInfo);
	openCL.RunKernel2(gfx.windowWidth, gfx.windowHeight);
	openCL.queue.finish();
}

void Game::RenderScene()
{
	// loop through all object sets
	for (s = 0; s < scene.objectSets.count; s++) 
	{
		// get reference to current object set
		ObjectSet& objSet = *(scene.objectSets.GetSetByIndex(s));
		
		// loop through all objects in this set
		for (o = 0; o < objSet.count; o++) 
		{
			// do primary ray computations
			ComputeStage1(*(objSet.ObjectByIndex(o)));
		}
	}

	// lighting computations
	ComputeStage2();
}

void Game::ComposeFrame()
{
	// TODO: refactor code for pixel-blocks
	// TODO: use OpenCL image types

	// handle keyboard/mouse actions
	HandleInput();

	// reset/update some stuff
	BeginActions();

	// give OCL control of OGL framebuffer
	gfx.AcquireBackBuff(openCL.queue());

	// render the 3D scene using OCL
	RenderScene();

	// make OCL release control of OGL memory
	gfx.ReleaseBackBuff(openCL.queue());
}