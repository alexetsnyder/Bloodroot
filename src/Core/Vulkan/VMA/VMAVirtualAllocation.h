#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <VMA/vk_mem_alloc.h>

#include <iostream>

namespace Core::VMA
{
	class VMAVirtualAllocation
	{
		public:
			VMAVirtualAllocation(VmaVirtualBlock block, size_t allocationSize)
			{
				this->block = block;
				this->size = allocationSize;

				VmaVirtualAllocationCreateInfo allocCreateInfo
				{
					.size = this->size,
					.alignment = 16,
				};

				auto result = vmaVirtualAllocate(this->block, &allocCreateInfo, &allocation, &offset);

				if (result != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to allocate from virtual block!");
				}
			}

			VMAVirtualAllocation(VMAVirtualAllocation&& other) noexcept
			{
				block = other.block;
				allocation = other.allocation;
				offset = other.offset;
				size = other.size;

				other.block = VK_NULL_HANDLE;
				other.allocation = VK_NULL_HANDLE;
				other.offset = 0;
				other.size = 0;
			}

			VMAVirtualAllocation& operator=(VMAVirtualAllocation&& other) noexcept
			{
				if (this != &other)
				{
					free();

					block = other.block;
					allocation = other.allocation;
					offset = other.offset;
					size = other.size;

					other.block = VK_NULL_HANDLE;
					other.allocation = VK_NULL_HANDLE;
					other.offset = 0;
					other.size = 0;
				}

				return *this;
			}

			VMAVirtualAllocation(const VMAVirtualAllocation&) = delete;
			VMAVirtualAllocation& operator=(const VMAVirtualAllocation&) = delete;

			~VMAVirtualAllocation()
			{
				free();
			}

			VkDeviceSize Offset() const { return offset; }
			size_t Size() const { return size; }

		private:
			VmaVirtualBlock block = nullptr;
			VmaVirtualAllocation allocation = VK_NULL_HANDLE;
			VkDeviceSize offset = 0;
			size_t size = 0;

			void free() const
			{
				if (allocation != VK_NULL_HANDLE)
				{
					vmaVirtualFree(block, allocation);
				}
			}
	};
}