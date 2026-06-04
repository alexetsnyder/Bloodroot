#pragma once

#include <stb_image.h>

#include <string>

namespace Core
{
	class Image
	{
		public:
			Image(const std::string& filePath);
			~Image();

			uint32_t Width() const { return width; }
			uint32_t Height() const { return height; }
			const stbi_uc* const Data() const { return pixels; }
			uint32_t getMipLevels() const;

		private:
			int width, height, channels;
			stbi_uc* pixels = nullptr;
	};
}