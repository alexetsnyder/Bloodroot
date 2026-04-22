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
			Window(int width, int height, const std::string& title);
			//Window(Window&& other) noexcept;
			~Window();

			void getSize(int& width, int& height) const;
			void setRenderer(IRenderer* renderer);

			VkResult createWindowSurface(const VkInstance& instance, VkSurfaceKHR& surface) const;
			bool windowShouldClose();

			Window(const Window&) = delete;
			Window& operator=(const Window&) = delete;

		private:
			GLFWwindow* window = nullptr;
			IRenderer* renderer;

			static void framebufferResizedCallback(GLFWwindow* window, int width, int height);
	};
}
