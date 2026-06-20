#include "Image.h"

#include <SDL3_image/SDL_image.h>

#include <cmath>
#include <stdexcept>

namespace Core
{
	Image::Image(const std::string& filePath)
	{
		SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());

		if (!loadedSurface)
		{
			SDL_Log("Failed to load image: %s", SDL_GetError());
			throw std::runtime_error("Failed to load texture image!");
		}

		imageSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_ABGR8888);
		SDL_DestroySurface(loadedSurface);
	}

	Image::~Image()
	{
		if (imageSurface)
		{
			SDL_DestroySurface(imageSurface);
		}
	}

	uint32_t Image::getMipLevels() const
	{
		if (imageSurface)
		{
			return static_cast<uint32_t>(std::floor(std::log2(std::max(Width(), Height())))) + 1;
		}

		return 0;
	}
}