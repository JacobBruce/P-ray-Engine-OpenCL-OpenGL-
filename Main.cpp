#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <iostream>
#include "Game.h"

std::unordered_map<std::string,std::string> GLOBALS::config_map;

int main(void)
{
	glfwSetErrorCallback(error_callback);

	cout << "Initializing OpenGL ... ";
	if (!glfwInit()) { 
		cout << "Failed!\n";
		std::cin.get();
		exit(EXIT_FAILURE);
	} else {
		cout << "Success!\n";
	}

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_DEPTH_BITS, 0);
	glfwWindowHint(GLFW_STENCIL_BITS, 0);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	LoadConfigFile("Data//settings.cfg");

	int windowWidth = stoi(GLOBALS::config_map["WINDOW_WIDTH"]);
	int windowHeight = stoi(GLOBALS::config_map["WINDOW_HEIGHT"]);
	int windowMode = stoi(GLOBALS::config_map["WINDOW_MODE"]);

	GLFWmonitor* monitor;
	GLFWwindow* window;

	if (GLOBALS::config_map["MONITOR_INDEX"] == "primary") {
		monitor = glfwGetPrimaryMonitor();
	} else {
		int monitorCount;
		int monitorIndex = stoi(GLOBALS::config_map["MONITOR_INDEX"]);
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		if (monitorIndex > 0 && monitorIndex < monitorCount) {
			monitor = monitors[monitorIndex];
		} else {
			cout << "Error: Invalid monitor index (check settings)\n";
			std::cin.get();
			exit(EXIT_FAILURE);
		}
	}

	const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);

	cout << "Creating OpenGL window ... ";
	switch (windowMode) {
	case 1:
		window = glfwCreateWindow(windowWidth, windowHeight, WINDOW_TITLE, monitor, NULL);
		break;
	case 2:
		glfwWindowHint(GLFW_RED_BITS, vidMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, vidMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, vidMode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, vidMode->refreshRate);
		window = glfwCreateWindow(vidMode->width, vidMode->height, WINDOW_TITLE, monitor, NULL);
		break;
	default:	
		window = glfwCreateWindow(windowWidth, windowHeight, WINDOW_TITLE, NULL, NULL);
		if (windowWidth < vidMode->width && windowHeight < vidMode->height) {
			int x_diff = vidMode->width - windowWidth;
			int y_diff = vidMode->height - windowHeight;
			glfwSetWindowPos(window, x_diff/2.0f, y_diff/2.0f);
		}
		break;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

    if (!window) {
		cout << "Failed!\n";
		error_exit();
    } else {
		cout << "Success!\n";
	}

	cout << "Initializing GLEW ... ";
	GLenum err = glewInit();

	if (err != GLEW_OK) 
	{
		cout << "Failed!\nError: " << glewGetErrorString(err);
		error_exit(window);
	} else {
		cout << "Success!\n";
	}

	//TODO: apply window icon
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//GLFWimage images[2];
	//images[0] = load_icon("__TemplateIcon.png");
	//images[1] = load_icon("32x32.ico");
	//glfwSetWindowIcon(window, 2, images);

	Game theGame(window, kServ, mServ);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window))
    {   
        theGame.Go();
        glfwPollEvents();
    }

	cout << "Stopping engine ..." << endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}