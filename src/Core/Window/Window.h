#pragma once

#include "AppData.h"
#include "IRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Vulkan/vulkan_raii.hpp>
#include <glm/glm.hpp>

#include <string>

namespace Core
{
	class Window
	{
		public:
			Window(AppData* appData, int width, int height, const std::string& title);
			~Window();

			void getSize(int& width, int& height) const;

			VkResult createWindowSurface(const VkInstance& instance, VkSurfaceKHR& surface) const;
			bool windowShouldClose();
			void pollEvents();

			GLFWwindow* windowPtr() { return window; }

			Window(const Window&) = delete;
			Window& operator=(const Window&) = delete;

		private:
			GLFWwindow* window = nullptr;

			static void framebufferResizedCallback(GLFWwindow* window, int width, int height);
			static void mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn);
			static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	};
}
