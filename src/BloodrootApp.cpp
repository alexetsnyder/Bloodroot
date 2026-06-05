#include "BloodrootApp.h"

#include "ChunkIndicies.h"
#include "GLMExtensions.h"
#include "RLEncoding.h"
#include "WorldGen.h"

#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <vector>

BloodrootApp::BloodrootApp()
	: window(&appData, WINDOW_WIDTH, WINDOW_HEIGHT, "Bloodroot App!"),
	  renderer(window, glfwInstance.getRequiredInstanceExtensions()),
	  camera(glm::vec3(8.0f, 68.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	  appData{ .renderer = &renderer, .camera = &camera }
{
	auto chunkIndicies = Game::ChunkIndicies{ { Game::CHUNK_WIDTH, Game::CHUNK_HEIGHT, Game::CHUNK_DEPTH } };

	renderer.SendIndexData(chunkIndicies.Indicies());

	//{ 320, Game::CHUNK_HEIGHT, 320 }
	auto worldGen = Game::WorldGen{ { 0, 0, 0 }, { 64, Game::CHUNK_HEIGHT, 64 } };

	worldGen.GenerateChunks(chunks);

	std::cout << "Finished Generating Chunks!\n";

	std::unordered_map<glm::i32vec3, Core::VK::Mesh> meshes;
	std::unordered_map<glm::i32vec3, Core::VK::Mesh> tMeshes;
	worldGen.GenerateMeshes(chunks, meshes, tMeshes);

	std::cout << "Finished Generating Meshes!\n";

	for (const auto& [chunkId, mesh] : meshes)
	{
		if (!mesh.IsEmpty())
		{
			renderer.AddOpaqueMesh(chunkId, mesh.ConstIndexCount(), chunks[chunkId].Position(), mesh.Verticies());
		}
	}

	for (const auto& [chunkId, tMesh] : tMeshes)
	{
		if (!tMesh.IsEmpty())
		{
			renderer.AddTransparentMesh(chunkId, tMesh.ConstIndexCount(), chunks[chunkId].Position(), tMesh.Verticies());
		}
	}

	//renderer.FlushCommandBuffer();

	std::cout << "Finished Allocating Meshes!\n";
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

		renderer.drawFrame(window, camera.Position(), camera.getViewMatrix());
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
