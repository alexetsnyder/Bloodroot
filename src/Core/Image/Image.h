#pragma once

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>

namespace Core
{
	class Image
	{
		public:
			Image(const std::string& filePath);
			~Image();

			int Width() const { return width; }
			int Height() const { return height; }
			const stbi_uc* const Data() const { return pixels; }
			uint32_t getMipLevels();

		private:
			int width, height, channels;
			stbi_uc* pixels = nullptr;
	};
}