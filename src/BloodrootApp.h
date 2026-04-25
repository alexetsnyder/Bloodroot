#pragma once

#include "GLFWInstance.h"
#include "Window.h"
#include "VulkanRenderer.h"

class BloodrootApp
{
	public:
		const int WINDOW_WIDTH = 800;
		const int WINDOW_HEIGHT = 600;

		BloodrootApp();
		void run();

	private:
		Core::GLFWInstance glfwInstance;
		Core::Window window;
		Core::VulkanRenderer renderer;

		void mainLoop();
};
