#pragma once

#include "AppData.h"
#include "Camera.h"
#include "Chunk.h"
#include "GLMExtensions.h"
#include "SDL3Instance.h"
#include "Window.h"
#include "Voxel.h"
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
		Core::SDL::SDL3Instance sdl3Instance;
		bool isTabPressed = false;
		Core::SDL::Window window;
		Core::VK::VulkanRenderer renderer;
		Game::Camera camera;
		std::unordered_map<glm::i32vec3, Game::Chunk> chunks;

		void mainLoop();
		void handleEvents(const SDL_Event& event);
		void processInput(float deltaTime);
		void handleMouseClick();

		bool raycast(const glm::vec3& origin, const glm::vec3& direction, Game::VoxelType& outVoxel);
		float intbound(float s, float ds);
		int32_t signum(float x);
		float ceil(float s);
};
