#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace Core
{
	class GLFWInstance
	{
		public:
			GLFWInstance();
			~GLFWInstance();

			std::vector<const char*> getRequiredInstanceExtensions();

			GLFWInstance(const GLFWInstance&) = delete;
			GLFWInstance& operator=(const GLFWInstance&) = delete;
	};
}
