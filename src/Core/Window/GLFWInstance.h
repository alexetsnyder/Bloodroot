#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Core
{
	class GLFWInstance
	{
		public:
			GLFWInstance();
			~GLFWInstance();

			GLFWInstance(const GLFWInstance&) = delete;
			GLFWInstance& operator=(const GLFWInstance&) = delete;
	};
}
