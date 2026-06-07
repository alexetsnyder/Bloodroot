#pragma once

#include "Image.h"
#include "VMAAllocator.h"

#include <vulkan/vulkan_raii.hpp>
#include <VMA/vk_mem_alloc.h>

#include <cstddef>
#include <vector>

namespace Core::VK::VMA
{
	class VMAImage
	{
		public:
			VMAImage(std::nullptr_t);
			VMAImage(uint32_t width,
					 uint32_t height,
					 uint32_t mipLevels,  
					 uint32_t layerCount,
					 vk::Format format,
					 vk::ImageUsageFlags usage);

			VMAImage(VMAImage&& other) noexcept;
			VMAImage& operator=(VMAImage&& other) noexcept;

			VMAImage(const VMAImage&) = delete;
			VMAImage& operator=(const VMAImage&) = delete;

			~VMAImage();

			const vk::Image& Get() const { return vkImage; }
			VmaAllocation& Allocation() { return allocation; }

		private:
			VkImage vkImage = VK_NULL_HANDLE;
			VmaAllocation allocation = VK_NULL_HANDLE;

			void free() const;
	};
}