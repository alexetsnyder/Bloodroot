#pragma once

#include "Image.h"

#include <vulkan/vulkan_raii.hpp>
#include <VMA/vk_mem_alloc.h>

#include <vector>

namespace Core::VMA
{
	class VMAImage
	{
		public:
			VMAImage();
			VMAImage(uint32_t width,
					 uint32_t height,
					 uint32_t mipLevels,
					 uint32_t layerCount,
					 vk::Format format,
					 vk::ImageUsageFlags usage,
					 vk::MemoryPropertyFlags properties);

			VMAImage(VMAImage&& other) noexcept;
			VMAImage& operator=(VMAImage&& other) noexcept;

			VMAImage(const VMAImage&) = delete;
			VMAImage& operator=(const VMAImage&) = delete;

			~VMAImage();



		private:
			VkImage vkImage = VK_NULL_HANDLE;
			//vk::raii::ImageView vkImageView = VK_NULL_HANDLE;
			//VkImageView vkImageView = VK_NULL_HANDLE;
			VmaAllocation allocation = VK_NULL_HANDLE;
			VmaAllocator allocator = VK_NULL_HANDLE;

			void free() const;
	};
}