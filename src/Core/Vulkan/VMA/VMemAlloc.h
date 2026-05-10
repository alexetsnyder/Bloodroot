#pragma once

#include <Vulkan/vulkan_raii.hpp>

#include <vma/vk_mem_alloc.h>

namespace Core::raii
{
	class VMemAlloc
	{
		public:
			VMemAlloc();
			VMemAlloc(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device, const vk::raii::Instance& instance);
			VMemAlloc(VMemAlloc&& other) noexcept;
			VMemAlloc& operator=(VMemAlloc&& other) noexcept;
			~VMemAlloc();

			VmaAllocator Allocator() const { return allocator; }

			VMemAlloc(const VMemAlloc&) = delete;
			VMemAlloc& operator=(const VMemAlloc&) = delete;
			
		private:
			VmaAllocator allocator = VK_NULL_HANDLE;
	};
}