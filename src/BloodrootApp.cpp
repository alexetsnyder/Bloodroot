#include "BloodrootApp.h"

#include <GLFW/glfw3.h>

#include <chrono>
#include <vector>

BloodrootApp::BloodrootApp()
	: renderer(window, glfwInstance.getRequiredInstanceExtensions()),
	  camera(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	  appData{ .renderer = &renderer, .camera = &camera },
	  window(&appData, WINDOW_WIDTH, WINDOW_HEIGHT, "Bloodroot App!")  
{
	
}

void BloodrootApp::run()
{
	mainLoop();
}

void BloodrootApp::mainLoop()
{
	auto lastFrame = std::chrono::high_resolution_clock::now();

	while (!window.windowShouldClose())
	{
		auto currentFrame = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;

		processInput(deltaTime);

		window.pollEvents();
		renderer.drawFrame(window, camera.getViewMatrix());
	}

	renderer.waitIdle();
}

void BloodrootApp::processInput(float deltaTime)
{
	if (glfwGetKey(window.windowPtr(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window.windowPtr(), true);
	}

	if (glfwGetKey(window.windowPtr(), GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processKeyboard(Game::CameraMovement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window.windowPtr(), GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyboard(Game::CameraMovement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window.windowPtr(), GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyboard(Game::CameraMovement::LEFT, deltaTime);
	}
	if (glfwGetKey(window.windowPtr(), GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyboard(Game::CameraMovement::RIGHT, deltaTime);
	}
}
