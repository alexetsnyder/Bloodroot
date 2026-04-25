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

	std::vector<const char*> GLFWInstance::getRequiredInstanceExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensions;
	}
}