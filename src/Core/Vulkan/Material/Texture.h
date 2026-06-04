#pragma once

#include "Image.h"
#include "VMAImage.h"

#include <Vulkan/vulkan_raii.hpp>

#include <vector>

namespace Core::VK::MAT
{
	class Texture
	{
		public:
			Texture();
			Texture(const vk::raii::Device& device,
					const vk::raii::CommandBuffer& commandBuffer,
					const Image& image,
					vk::Format format,
					vk::ImageUsageFlags usage,
					vk::MemoryPropertyFlags properties,
					float maxSamplerAnisotropy);

			Texture(const vk::raii::Device& device,
					const vk::raii::CommandBuffer& commandBuffer,
					const std::vector<Image>& images,
					uint32_t layerCount,
					vk::Format format,
					vk::ImageUsageFlags usage,
					vk::MemoryPropertyFlags properties,
					float maxSamplerAnisotropy);

			Texture(const Texture&) = delete;
			Texture& operator=(const Texture&) = delete;

			~Texture();
		
		private:
			VMA::VMAImage vmaImage;
			vk::raii::ImageView textureImageView = nullptr;
			vk::raii::Sampler textureSampler = nullptr;
	};
}