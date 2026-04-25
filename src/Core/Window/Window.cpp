#include "Window.h"

#include <iostream>
#include <stdexcept>

namespace Core
{
	Window::Window(int width, int height, const std::string& title)
	{	
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		if (!window)
		{
			throw std::runtime_error("Failed to create GLFW Window!");
		}

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

	void Window::setRenderer(IRenderer* renderer)
	{
		this->renderer = renderer;

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
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
		auto windowClass = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		windowClass->renderer->onResize(width, height);
	}
}
