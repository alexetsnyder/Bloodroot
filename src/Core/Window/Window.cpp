#include "Window.h"

#include <iostream>
#include <stdexcept>

namespace Core
{
	Window::Window(AppData* appData, int width, int height, const std::string& title)
	{	
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		if (!window)
		{
			throw std::runtime_error("Failed to create GLFW Window!");
		}

		glfwSetWindowUserPointer(window, appData);
		glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
		glfwSetCursorPosCallback(window, mouseCallback);
		glfwSetScrollCallback(window, scrollCallback);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		std::cout << "Window Created!\n";
	}

	Window::~Window()
	{
		std::cout << "Window Destroyed!\n";

		if (window)
		{
			glfwDestroyWindow(window);
		}
	}

	void Window::getSize(int& width, int& height) const
	{
		glfwGetFramebufferSize(window, &width, &height);
	}

	VkResult Window::createWindowSurface(const VkInstance& instance, VkSurfaceKHR& surface) const
	{
		return glfwCreateWindowSurface(instance, window, nullptr, &surface);
	}

	bool Window::windowShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void Window::pollEvents()
	{
		glfwPollEvents();
	}

	void Window::framebufferResizedCallback(GLFWwindow* window, int width, int height)
	{
		auto appData = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));
		appData->renderer->onResize(width, height);
	}

	void Core::Window::mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
	{
		static float lastX = 0.0f;
		static float lastY = 0.0f;
		static bool firstMouse = true;

		float xPos = static_cast<float>(xPosIn);
		float yPos = static_cast<float>(yPosIn);

		if (firstMouse)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}

		float xOffset = xPos - lastX;
		float yOffset = lastY - yPos;

		lastX = xPos;
		lastY = yPos;

		auto appData = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));
		appData->camera->processMouseMovement(xOffset, yOffset);
	}

	void Core::Window::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		auto appData = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));
		appData->camera->processMouseScroll(static_cast<float>(yOffset));
	}
}
