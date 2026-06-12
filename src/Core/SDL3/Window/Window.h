#pragma once

#include "AppData.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <stdexcept>
#include <string>

#include <Vulkan/vulkan_raii.hpp>

namespace Core::SDL
{
	class Window
	{
	public:
		Window(AppData* appData, uint32_t width, uint32_t height, const std::string& title)
		{
			if (window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE); window == nullptr)
			{
				SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
				throw std::runtime_error("SDL window could not be created.");
			}
		}

		~Window()
		{
			SDL_DestroyWindow(window);
			window = nullptr;
		}

		bool PollEvent(SDL_Event& event)
		{
			return SDL_PollEvent(&event);
		}

		void HandleWindowEvent(const SDL_Event& event)
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
					quit = true;
					break;
			}
		}

		bool Quit() const { return quit; }
		void CloseWindow() { quit = true; }

		bool createWindowSurface(const VkInstance& instance, VkSurfaceKHR& surface) const
		{
			if (SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface) == false)
			{
				SDL_Log("Failed to create window surface! Error: %s\n", SDL_GetError());
				return false;
			}

			return true;
		}

		bool GetSize(int& width, int& height) const
		{
			return SDL_GetWindowSize(window, &width, &height);
		}

		bool IsRelativeMouse() const { return isRelativeMouse; }
		void SetRelativeMouse(bool isRelativeMouse)
		{
			this->isRelativeMouse = isRelativeMouse;
			SDL_SetWindowRelativeMouseMode(window, this->isRelativeMouse);
		}

		bool IsMinimized() const { return isMinimized; }
		void SetIsMinimized(bool isMinimized) { this->isMinimized = isMinimized; }

		private:
			bool quit = false;
			bool isMinimized = false;
			bool isRelativeMouse = true;
			SDL_Window* window = nullptr;
	};
}
