#include "GLFWInstance.h"

#include <iostream>
#include <stdexcept>

namespace Core
{
	GLFWInstance::GLFWInstance()
	{
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initalize GLFW!");
		}

		std::cout << "GLFW initialized!\n";
	}

	GLFWInstance::~GLFWInstance()
	{
		std::cout << "GLFW cleaned up!\n";

		glfwTerminate();
	}
}