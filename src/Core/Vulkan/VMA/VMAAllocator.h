#pragma once

#include <Vulkan/vulkan_raii.hpp>

#include <vma/vk_mem_alloc.h>

namespace Core::VK::VMA
{
	class VMAAllocator
	{
		public:
			VMAAllocator();
			VMAAllocator(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device, const vk::raii::Instance& instance);
			VMAAllocator(VMAAllocator&& other) noexcept;
			VMAAllocator& operator=(VMAAllocator&& other) noexcept;
			VMAAllocator(const VMAAllocator&) = delete;
			VMAAllocator& operator=(const VMAAllocator&) = delete;
			~VMAAllocator();

			VmaAllocator Allocator() const { return allocator; }

		private:
			VmaAllocator allocator = VK_NULL_HANDLE;

			void free();
	};
}