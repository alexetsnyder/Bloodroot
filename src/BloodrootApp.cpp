#include "BloodrootApp.h"

#include "ChunkIndicies.h"
#include "GLMExtensions.h"
#include "Quad.h"
#include "RLEncoding.h"
#include "WorldGen.h"

#include <chrono>
#include <iostream>
#include <vector>

BloodrootApp::BloodrootApp()
	: window(&appData, WINDOW_WIDTH, WINDOW_HEIGHT, "Bloodroot App!"),
	  renderer(window, sdl3Instance.GetRequiredInstanceExtensions()),
	  camera(glm::vec3(8.0f, 68.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	  appData{ .renderer = &renderer, .camera = &camera }
{
	window.SetRelativeMouse(true);

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
			std::cout << "Mouse Button Clicked!\n";
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
