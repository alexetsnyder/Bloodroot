#pragma once

#include "AppData.h"
#include "Camera.h"
#include "GLFWInstance.h"
#include "GLMExtensions.h"
#include "MeshGen.h"
#include "Window.h"
#include "VulkanRenderer.h"

#include <map>

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
		Core::VulkanRenderer renderer;
		Game::Camera camera;
		std::map<glm::i32vec3, Game::Chunk, Core::Ext::I32Vec3Comparator> chunks;

		void mainLoop();
		void processInput(float deltaTime);	
};
