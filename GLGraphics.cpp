#include "GLGraphics.h"

void GLGraphics::Initialize(GLFWwindow* pWindow, cl_context& context)
{
	window = pWindow;
	cl_con = context;
	cursorLocked = false;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	glGenFramebuffers(1, &gl_fb_id);
	glGenRenderbuffers(1, &gl_rb_id);
	glGenTextures(1, &gl_tex_id);

	glBindFramebuffer(GL_FRAMEBUFFER, gl_fb_id);
	glBindRenderbuffer(GL_RENDERBUFFER, gl_rb_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gl_rb_id);

	glBindTexture(GL_TEXTURE_2D, gl_tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_tex_id, 0);

	gl_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (gl_status != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Error: FrameBuffer is not complete.\n";
		error_exit(window);
	}

	gl_backBuff = clCreateFromGLTexture2D(cl_con, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, gl_tex_id, &cl_error);

	if (!gl_backBuff || cl_error != CL_SUCCESS)
	{
		cout << "Failed to create OpenGL texture reference!\n";
		error_exit(window);
	}

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glFinish();
}

void GLGraphics::ToggleCursorLock()
{
	if (cursorLocked) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	} else {
		glfwSetCursorPos(window, 0.0, 0.0);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	cursorLocked = !cursorLocked;
}

void GLGraphics::GetWindowSize(int* width, int* height)
{
    glfwGetFramebufferSize(window, width, height);
}

void GLGraphics::AcquireBackBuff(cl_command_queue& queue)
{
	cl_error = clEnqueueAcquireGLObjects(queue, 1, &gl_backBuff, 0, NULL, NULL);
	assert(cl_error == CL_SUCCESS);
}

void GLGraphics::ReleaseBackBuff(cl_command_queue& queue)
{
	cl_error = clEnqueueReleaseGLObjects(queue, 1, &gl_backBuff, 0, NULL, NULL);
	assert(cl_error == CL_SUCCESS);
}

void GLGraphics::SetWindowSize(int width, int height)
{
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
}

void GLGraphics::BeginFrame()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gl_fb_id);
	glClear(GL_COLOR_BUFFER_BIT);
	glFinish();
}

void GLGraphics::DisplayFrame()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gl_fb_id);
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glFinish();
	glfwSwapBuffers(window);
}