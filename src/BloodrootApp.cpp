#include "BloodrootApp.h"

#include "ChunkIndicies.h"
#include "RLEncoding.h"
#include "Voxel.h"

#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <vector>

//TODO: Build chunks locally and send posisition to renderer, and connect chunks to their allocation.  

BloodrootApp::BloodrootApp()
	: window(&appData, WINDOW_WIDTH, WINDOW_HEIGHT, "Bloodroot App!"),
	  renderer(window, glfwInstance.getRequiredInstanceExtensions()),
	  camera(glm::vec3(8.0f, 66.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	  appData{ .renderer = &renderer, .camera = &camera }
{
	std::vector<Game::VoxelType> voxelRow
	{
		Game::VoxelType::AIR,
		Game::VoxelType::AIR,
		Game::VoxelType::AIR,
		Game::VoxelType::GRASS,
		Game::VoxelType::DIRT,
		Game::VoxelType::DIRT,
		Game::VoxelType::STONE,
		Game::VoxelType::STONE,
		Game::VoxelType::STONE,
		Game::VoxelType::STONE,
		Game::VoxelType::BEDROCK,
	};

	std::vector<uint16_t> encodedData{};

	Core::Math::RLEncoding::Encode<Game::VoxelType>(voxelRow, encodedData);
	Core::Math::RLEncoding::Print<uint16_t>(encodedData);

	for (int i = 0; i < voxelRow.size(); i++)
	{
		std::cout << Core::Math::RLEncoding::GetData<Game::VoxelType>(encodedData, i) << std::endl;
	}

	std::vector<Game::VoxelType> decodedData = Core::Math::RLEncoding::Decode<Game::VoxelType>(encodedData);
	Core::Math::RLEncoding::Print<Game::VoxelType>(decodedData);

	auto chunkIndicies = Game::ChunkIndicies{ { 16, 16, 16 } };

	renderer.SendIndexData(chunkIndicies.Indicies());

	std::vector<Game::ChunkMesh> chunkMeshes;

	auto meshGen = Game::MeshGen{ { 0, 0, 0 }, { 64, 64, 64 }, { 16, 16, 16 } };

	meshGen.GenerateChunkMeshes(chunkMeshes);

	for (auto& chunkMesh : chunkMeshes)
	{
		auto& chunk = chunkMesh.chunk;
		auto& mesh = chunkMesh.mesh;

		chunks[chunk.ChunkId()] = chunk;
		if (!mesh.IsEmpty())
		{
			renderer.SendVertexData(chunk.UniqueId(), mesh.IndexCount(), chunk.Position(), mesh.Verticies());
		}
	}
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
