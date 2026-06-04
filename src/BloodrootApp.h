#pragma once

#include "AppData.h"
#include "Camera.h"
#include "Chunk.h"
#include "GLFWInstance.h"
#include "GLMExtensions.h"
#include "Window.h"
#include "VulkanRenderer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>

class BloodrootApp
{
	public:
		const int WINDOW_WIDTH = 800;
		const int WINDOW_HEIGHT = 600;

		BloodrootApp();
		void run();

	private:
		AppData appData;
		Core::GLFWInstance glfwInstance;
		Core::Window window;
		Core::VK::VulkanRenderer renderer;
		Game::Camera camera;
		std::unordered_map<glm::i32vec3, Game::Chunk> chunks;

		void mainLoop();
		void processInput(float deltaTime);	
};
