#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cmath>
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

	uint32_t Image::getMipLevels() const
	{
		if (pixels)
		{
			return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
		}

		return 0;
	}
}