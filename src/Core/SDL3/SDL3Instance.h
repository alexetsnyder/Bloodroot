#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <iostream>
#include <stdexcept>
#include <vector>

namespace Core::SDL
{
	class SDL3Instance
	{
		public:
			SDL3Instance()
			{
				if (SDL_Init(SDL_INIT_VIDEO) == false)
				{
					SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
					throw std::runtime_error("SDL could not initialize!");
				}
			}

			~SDL3Instance()
			{
				SDL_Quit();
			}

			std::vector<const char*> GetRequiredInstanceExtensions()
			{
				uint32_t instanceCount;
				auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&instanceCount);

				std::vector<const char*> extensions(sdlExtensions, sdlExtensions + instanceCount);

				return extensions;
			}

		private:
	};
}