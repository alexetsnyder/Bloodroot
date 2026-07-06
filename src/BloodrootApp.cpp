#include "BloodrootApp.h"

#include "ChunkIndicies.h"
#include "GLMExtensions.h"
#include "Quad.h"
#include "Random.h"
#include "RLEncoding.h"

#include <array>
#include <chrono>
#include <iostream>
#include <vector>

constexpr uint32_t WORLD_WIDTH = 64; //64; 320;
constexpr uint32_t WORLD_DEPTH = 64; //64; 320;
constexpr uint32_t WORLD_HEIGHT = Game::CHUNK_HEIGHT;

BloodrootApp::BloodrootApp()
	: window(&appData, WINDOW_WIDTH, WINDOW_HEIGHT, "Bloodroot App!"),
	  renderer(window, sdl3Instance.GetRequiredInstanceExtensions()),
	  camera(glm::vec3(8.0f, 68.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	  appData{ .renderer = &renderer, .camera = &camera },
	  worldGen{ { 0, 0, 0 }}
{
	window.SetRelativeMouse(true);

	for (int i = 0; i < 5; i++)
	{
		randomTest();
	}

	auto chunkIndicies = Game::ChunkIndicies{ { Game::CHUNK_WIDTH, Game::CHUNK_HEIGHT, Game::CHUNK_DEPTH } };

	renderer.SendIndexData(chunkIndicies.Indicies());

	auto worldSize = glm::i32vec3{ WORLD_WIDTH, WORLD_HEIGHT, WORLD_DEPTH };

	auto buildInfo = Game::BuildInfo
	{
		.startPos = { -worldSize.x / 2, 0, -worldSize.z / 2 },
		.endPos = { worldSize.x / 2, worldSize.y, worldSize.z / 2 },
		.size = worldSize
	};

	worldGen.GenerateChunks(buildInfo, chunks);

	std::cout << "Finished Generating Chunks!\n";

	std::unordered_map<glm::i32vec3, Core::VK::Mesh> meshes;
	std::unordered_map<glm::i32vec3, Core::VK::Mesh> tMeshes;
	worldGen.GenerateMeshes(buildInfo, chunks, meshes, tMeshes);

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

	auto mesh = Core::VK::Quad::GenerateMesh();
	renderer.AddGuiMesh(mesh.IndexCount(), mesh.Verticies());

	renderer.FlushCommandBuffer();

	std::cout << "Finished Allocating Meshes!\n";
}

void BloodrootApp::run()
{
	mainLoop();
}

void BloodrootApp::mainLoop()
{
	auto lastFrame = std::chrono::high_resolution_clock::now();

	while (!window.Quit())
	{
		auto currentFrame = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;

		processInput(deltaTime);

		SDL_Event event;
		while (window.PollEvent(event))
		{
			window.HandleWindowEvent(event);
			handleEvents(event);
		}

		if (!window.IsMinimized())
		{
			renderer.drawFrame(window, camera.Position(), camera.getViewMatrix());
		}
	}

	renderer.waitIdle();
}

void BloodrootApp::handleEvents(const SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_EVENT_WINDOW_RESIZED:
			renderer.onResize(event.window.data1, event.window.data2);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			camera.processMouseMovement(event.motion.xrel, -event.motion.yrel);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				handleMouseClick();
			}
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			camera.processMouseScroll(event.wheel.y);
			break;
		case SDL_EVENT_WINDOW_MINIMIZED:
			window.SetIsMinimized(true);
			break;
		case SDL_EVENT_WINDOW_RESTORED:
			window.SetIsMinimized(false);
			break;
		case SDL_EVENT_KEY_DOWN:
			if (event.key.key == SDLK_TAB)
			{
				if (!isTabPressed)
				{
					isTabPressed = true;
					window.SetRelativeMouse(!window.IsRelativeMouse());
				}
			}
			break;
		case SDL_EVENT_KEY_UP:
			if (event.key.key == SDLK_TAB)
			{
				isTabPressed = false;
			}
			break;
	}
}

void BloodrootApp::processInput(float deltaTime)
{
	const bool* keyStates = SDL_GetKeyboardState(nullptr);

	if (keyStates[SDL_SCANCODE_ESCAPE])
	{
		window.CloseWindow();
	}

	if (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP])
	{
		camera.processKeyboard(Game::CameraMovement::FORWARD, deltaTime);
	}

	if (keyStates[SDL_SCANCODE_S] || keyStates[SDL_SCANCODE_DOWN])
	{
		camera.processKeyboard(Game::CameraMovement::BACKWARD, deltaTime);
	}

	if (keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT])
	{
		camera.processKeyboard(Game::CameraMovement::LEFT, deltaTime);
	}

	if (keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT])
	{
		camera.processKeyboard(Game::CameraMovement::RIGHT, deltaTime);
	}
}

void BloodrootApp::handleMouseClick()
{
	Game::PHYS::VoxelCollision collision;
	if (raycast(camera.Position(), camera.Front(), collision))
	{
		std::cout << "Solid Voxel Selected: " << collision << std::endl;

		auto chunkId = Game::Chunk::MapToChunkId(collision.Position());

		chunks[chunkId].SetVoxel(collision.Position(), Game::VoxelType::AIR);

		std::vector<glm::i32vec3> chunksToUpdate;

		chunksToUpdate.push_back(chunkId);

		worldGen.GetAdjChunks(collision.Position(), chunksToUpdate);

		for (const auto& cId : chunksToUpdate)
		{
			if (!chunks.contains(cId))
			{
				continue;
			}

			auto mesh = Core::VK::Mesh{};
			auto tMesh = Core::VK::Mesh{};
			worldGen.GenerateMesh(chunks, chunks[cId], mesh, tMesh);

			if (!mesh.IsEmpty())
			{
				renderer.AddOpaqueMesh(cId, mesh.IndexCount(), chunks[cId].Position(), mesh.Verticies());
			}

			if (!tMesh.IsEmpty())
			{
				renderer.AddTransparentMesh(cId, tMesh.IndexCount(), chunks[cId].Position(), tMesh.Verticies());
			}
		}
		
		renderer.FlushCommandBuffer();
	}
}

/// <summary>
/// Raycast Algorith based on: 
/// http://www.cse.yorku.ca/~amana/research/grid.pdf and
/// https://gist.github.com/dogfuntom/cc881c8fc86ad43d55d8
/// </summary>
/// <param name="origin"></param>
/// <param name="direction"></param>
/// <param name="outVoxel"> First Solid Voxel it intersects </param>
/// <returns></returns>
bool BloodrootApp::raycast(const glm::vec3& origin, const glm::vec3& direction, Game::PHYS::VoxelCollision& collision)
{
	float radius = 6.0f;
	auto normDir = glm::normalize(direction);

	auto x = static_cast<int32_t>(std::floorf(origin.x));
	auto y = static_cast<int32_t>(std::floorf(origin.y));
	auto z = static_cast<int32_t>(std::floorf(origin.z));

	auto dx = normDir.x;
	auto dy = normDir.y;
	auto dz = normDir.z;

	auto stepX = signum(dx);
	auto stepY = signum(dy);
	auto stepZ = signum(dz);

	auto tMaxX = intbound(origin.x, dx);
	auto tMaxY = intbound(origin.y, dy);
	auto tMaxZ = intbound(origin.z, dz);

	auto tDeltaX = stepX / dx;
	auto tDeltaY = stepY / dy;
	auto tDeltaZ = stepZ / dz;

	radius /= std::sqrtf(dx * dx + dy * dy + dz * dz);

	while (tMaxX <= radius || tMaxY <= radius || tMaxZ <= radius)
	{
		glm::i32vec3 normal(0.0f);

		if (tMaxX < tMaxY)
		{
			if (tMaxX < tMaxZ)
			{
				x += stepX;
				tMaxX += tDeltaX;

				normal.x = -stepX;
			}
			else
			{
				z += stepZ;
				tMaxZ += tDeltaZ;

				normal.z = -stepZ;
			}
		}
		else
		{
			if (tMaxY < tMaxZ)
			{
				y += stepY;
				tMaxY += tDeltaY;

				normal.y = -stepY;
			}
			else
			{
				z += stepZ;
				tMaxZ += tDeltaZ;

				normal.z = -stepZ;
			}
		}

		auto chunkId = Game::Chunk::MapToChunkId({ x, y, z });

		if (chunks.contains(chunkId))
		{
			auto voxelType = chunks[chunkId].GetVoxelType({ x, y, z });

			if (voxelType != Game::VoxelType::AIR && voxelType != Game::VoxelType::WATER)
			{
				collision = Game::PHYS::VoxelCollision(voxelType, { x, y, z }, normal);
				return true;
			}
		}	
	}
	
	return false;
}

float BloodrootApp::intbound(float s, float ds)
{
	if (ds < 0 && std::roundf(s) == s)
	{
		return 0.0f;
	}

	return (ds > 0 ? ceil(s) - s : s - std::floorf(s)) / std::abs(ds);
}

int32_t BloodrootApp::signum(float x)
{
	return (x > 0 ? 1 : x < 1 ? -1 : 0);
}

float BloodrootApp::ceil(float s)
{
	return (s == 0.0f ? 1.0f : std::ceilf(s));
}

void BloodrootApp::randomTest()
{
	std::array<uint32_t, 6> distribution{};

	for (int i = 0; i < 400; i++)
	{
		auto randInt = Core::Math::Random::Instance().RandomInt(1, 6);
		int index = randInt - 1;

		distribution[index]++;
	}

	std::cout << "Random Histogram:\n";
	for (int i = 0; i < 6; i++)
	{
		std::cout << i + 1 << ") ";
		for (int j = 0; j < distribution[i]; j++)
		{
			std::cout << "*";
		}
		std::cout << std::endl;
	}
}
