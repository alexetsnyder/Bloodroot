#include "BloodrootApp.h"

#include "ChunkIndicies.h"
#include "RLEncoding.h"
#include "WorldGen.h"

#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <vector>

BloodrootApp::BloodrootApp()
	: window(&appData, WINDOW_WIDTH, WINDOW_HEIGHT, "Bloodroot App!"),
	  renderer(window, glfwInstance.getRequiredInstanceExtensions()),
	  camera(glm::vec3(8.0f, 66.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	  appData{ .renderer = &renderer, .camera = &camera }
{
	auto chunkIndicies = Game::ChunkIndicies{ { Game::CHUNK_WIDTH, Game::CHUNK_HEIGHT, Game::CHUNK_DEPTH } };

	renderer.SendIndexData(chunkIndicies.Indicies());

	auto worldGen = Game::WorldGen{ { 0, 0, 0 }, { 64, Game::CHUNK_HEIGHT, 64 } };

	worldGen.GenerateChunks(chunks);

	std::cout << "Finished Generating Chunks!\n";

	std::vector<Game::ChunkMesh> chunkMeshes;
	worldGen.GenerateMeshes(chunks, chunkMeshes);

	std::cout << "Finished Generating Meshes!\n";

	for (auto& chunkMesh : chunkMeshes)
	{
		auto chunkId = chunkMesh.chunkId;
		auto& mesh = chunkMesh.mesh;

		if (!mesh.IsEmpty())
		{
			renderer.SendVertexData(chunkId, mesh.IndexCount(), chunks[chunkId].Position(), mesh.Verticies());
		}
	}

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
