#pragma once

#include "IRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Vulkan/vulkan_raii.hpp>

#include <string>

namespace Core
{
	class Window
	{
		public:
			Window(int width, int height, const std::string& title, IRenderer* renderer);
			Window(Window&& other) noexcept;
			~Window();

			void getSize(int& width, int& height);

			VkResult createWindowSurface(const VkInstance& instance, VkSurfaceKHR& surface);
			bool windowShouldClose();

			Window(const Window&) = delete;
			Window& operator=(const Window&) = delete;

		private:
			GLFWwindow* window = nullptr;
			IRenderer* renderer = nullptr;

			static void framebufferResizedCallback(GLFWwindow* window, int width, int height);
	};
}
