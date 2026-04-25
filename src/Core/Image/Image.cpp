#include "Image.h"

#include <stdexcept>

namespace Core
{
	Image::Image(const std::string& filePath)
	{
		pixels = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (!pixels)
		{
			throw std::runtime_error("Failed to load texture image!");
		}
	}

	Image::~Image()
	{
		if (pixels)
		{
			stbi_image_free(pixels);
		}
	}
}