#pragma once
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include "ReadWrite.h"
#include "MathExt.h"
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <iostream>

#ifdef linux
    #include <GL/glx.h>
#endif

class CL
{
private:
	cl::Platform platform;
	cl::Device device;
	cl::Program program;
public:
	cl::NDRange local_range;
	cl::CommandQueue queue;
	cl::Context context;
	cl::Kernel CR_Kernel;
	cl::Kernel CS_Kernel;
	cl::Kernel CT_Kernel;
	cl::Kernel CL_Kernel;
	UINT32 max_wg_size;
public:
	void Initialize(unsigned char sub_rays, unsigned char t_depth)
	{
		cout << "Initializing OpenCL ... ";

		// get all platforms (drivers)
		vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if (all_platforms.size()==0) {
			HandleFatalError(30, "No platforms found. Check OpenCL installation!");
		}
		// select default platform
		platform=all_platforms[0];

		#ifdef linux
			cl_context_properties props[] = {
			CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
			CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
			CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0};
		#elif defined WIN32 || defined _WIN32
			cl_context_properties props[] = {
			CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
			CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
			CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0};
		#elif defined(__APPLE__) || defined(MACOSX)
			CGLContextObj glContext = CGLGetCurrentContext();
			CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);
			cl_context_properties props[] = {
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
			(cl_context_properties)shareGroup, 0};
		#endif

		context = cl::Context(CL_DEVICE_TYPE_GPU, props);
 
		// get compute devices on default platform
		vector<cl::Device> all_devices = context.getInfo<CL_CONTEXT_DEVICES>();
		if (all_devices.size()==0) {
			HandleFatalError(31, "No devices found. Check OpenCL installation!");
		}

		// select default compute device
		device=all_devices[0];

		// veryify CL/GL sharing is supported on device
		string dev_exts = device.getInfo<CL_DEVICE_EXTENSIONS>();
		if (dev_exts.find(CL_GL_SHARING_EXT) == string::npos) {
			HandleFatalError(35, "Device does not support CL/GL sharing!");
		} else if (device.getInfo<CL_DEVICE_IMAGE_SUPPORT>()!=CL_TRUE) {
			HandleFatalError(36, "Device does not support OpenCL images!");
		} else {
			cout << "Success!\n";
		}

		// Read kernel source file
		cl::Program::Sources sources;
		string sourceCode = readFile("Data\\kernels\\compute.cl");
		sources.push_back(make_pair(sourceCode.c_str(), sourceCode.length()+1));

		// Make program of the source code in the context
		program = cl::Program(context, sources);
 
		// build kernel program and check for errors
		cout << "Building OpenCL kernels ... ";
		if (program.build(all_devices)!=CL_SUCCESS) {
			cout << "Failed!\n";
			// log compiler output then stop the application
			CLBLog("Build log: "+program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));
			HandleFatalError(32, "Error building kernel program.");
		} else {
			cout << "Success!\n";
			//CLBLog("Build log: "+program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));
		}

		// initialize kernel objects
		switch (sub_rays) {
			case 0: CR_Kernel = cl::Kernel(program, "ComputeStage0x1"); break;
			case 1: CR_Kernel = cl::Kernel(program, "ComputeStage0x1"); break;
			default: CR_Kernel = cl::Kernel(program, "ComputeStage0xN"); break;
		}
		
		CT_Kernel = cl::Kernel(program, "ComputeStage1T");
		CS_Kernel = cl::Kernel(program, "ComputeStage1S");

		switch (t_depth) {
		case 1:
			CL_Kernel = cl::Kernel(program, "ComputeStage2x1");
			break;
		case 2:
			CL_Kernel = cl::Kernel(program, "ComputeStage2x2");
			break;
		case 3:
			CL_Kernel = cl::Kernel(program, "ComputeStage2x3");
			break;
		case 4:
			CL_Kernel = cl::Kernel(program, "ComputeStage2x4");
			break;
		default:
			HandleFatalError(33, "Invalid transparency depth: "+IntToStr(t_depth));
			break;
		}

		// create queue to which we will push commands for the device
		queue = cl::CommandQueue(context, device);

		// get maximum workgroup size for device
		max_wg_size = (cl_uint)device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

		// print OpenCL info to console
		PrintCLInfo();
	}
	void PrintCLInfo()
	{
		string device_name = device.getInfo<CL_DEVICE_NAME>();
		string device_vendor = device.getInfo<CL_DEVICE_VENDOR>();
		device_name.pop_back(); device_vendor.pop_back();

		cout << "\nUsing platform: "+platform.getInfo<CL_PLATFORM_NAME>() + "\n";
		cout << "Using device: "+device_name+" ("+device_vendor+")\n";
		cout << "OpenCL version: "+device.getInfo<CL_DEVICE_VERSION>() + "\n";
		cout << "Max parameter size: "+IntToStr((cl_uint)device.getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>())+" KB\n";
		cout << "Constant buffer size: "+DblToStr((cl_ulong)device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>()/1024)+" KB\n";
		cout << "Local memory size: "+DblToStr((cl_ulong)device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>()/1024)+" KB\n";
		cout << "Global memory size: "+DblToStr((cl_ulong)device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()/1024/1024)+" MB\n";
		cout << "Max compute units: "+IntToStr((cl_uint)device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>()) + "\n";
		cout << "Max workgroup size: "+IntToStr(max_wg_size) + "\n\n";
	}
	void RunKernel0(UINT32 ww, UINT32 wh)
	{
		queue.enqueueNDRangeKernel(CR_Kernel, cl::NullRange, cl::NDRange(ww, wh), local_range);
	}
	void RunKernel1(size_t mX, size_t mY, size_t bX, size_t bY)
	{
		queue.enqueueNDRangeKernel(CT_Kernel, cl::NDRange(mX, mY), cl::NDRange(bX, bY), local_range);
	}
	void RunKernel2(UINT32 ww, UINT32 wh)
	{
		queue.enqueueNDRangeKernel(CL_Kernel, cl::NullRange, cl::NDRange(ww, wh), local_range);
	}
};