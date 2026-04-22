#include "GLFWInstance.h"
#include "IRenderer.h"
#include "Window.h"
#include "VulkanRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

class HelloTriangleApp
{
	public:
		HelloTriangleApp()
			: window(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Triangel!"), 
			  renderer(window, getRequiredInstanceExtensions())
		{
			window.setRenderer(&renderer);
		}

		void run()
		{
			mainLoop();
		}

	private:
		Core::GLFWInstance glfwInstance;
		Core::Window window;
		Core::VulkanRenderer renderer;

		std::vector<const char*> getRequiredInstanceExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			
			std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
			if (enableValidationLayers)
			{
				extensions.push_back(vk::EXTDebugUtilsExtensionName);
			}

			return extensions;
		}

		void mainLoop()
		{
			while (!window.windowShouldClose())
			{
				glfwPollEvents();
				renderer.drawFrame(window);
			}

			renderer.waitIdle();
		}
};

int main()
{
	try
	{
		HelloTriangleApp app;
		app.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
} 