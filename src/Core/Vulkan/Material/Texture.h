#pragma once

#include "Image.h"
#include "VMAImage.h"

#include <Vulkan/vulkan_raii.hpp>

#include <cstddef>
#include <vector>

namespace Core::VK::MAT
{
	class Texture
	{
		public:
			Texture(std::nullptr_t);
			Texture(VmaAllocator allocator,
					const vk::raii::Device& device,
					const vk::raii::CommandBuffer& commandBuffer,
					const Image& image,
					vk::Format format,
					vk::ImageUsageFlags usage,
					vk::MemoryPropertyFlags properties,
					float maxSamplerAnisotropy);

			Texture(VmaAllocator allocator,
					const vk::raii::Device& device,
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
			VMA::VMAImage vmaImage = nullptr;
			vk::raii::ImageView textureImageView = nullptr;
			vk::raii::Sampler textureSampler = nullptr;
	};
}