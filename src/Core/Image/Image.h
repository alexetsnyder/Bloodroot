#pragma once

#include <SDL3/SDL_surface.h>

#include <string>

namespace Core
{
	class Image
	{
		public:
			Image(const std::string& filePath);
			~Image();

			uint32_t Width() const { return imageSurface->w; }
			uint32_t Height() const { return imageSurface->h; }
			const void* const Data() const { return imageSurface->pixels; }
			uint32_t getMipLevels() const;

		private:
			SDL_Surface* imageSurface;
	};
}