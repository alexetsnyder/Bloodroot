#pragma once

#include "VMAAllocator.h"

#include <vulkan/vulkan_raii.hpp>
#include <VMA/vk_mem_alloc.h>

#include <iostream>

namespace Core::VK::VMA
{
	class VMABuffer
	{
		public:
			VMABuffer() 
				: size{ 0 }
			{

			}

			VMABuffer(size_t size, VkBufferUsageFlags usage, VmaAllocationCreateFlags vmaFlags, VmaMemoryUsage memUsage)
			{
				this->size = size;

				VkBufferCreateInfo bufferInfo
				{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = size,
					.usage = usage,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				};

				VmaAllocationCreateInfo allocInfo
				{
					.flags = vmaFlags,
					.usage = memUsage,
				};

				auto result = vmaCreateBuffer(VMAAllocator::Instance().Get(), &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);

				if (result != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create Buffer!");
				}
			}

			VMABuffer(VMABuffer&& other) noexcept
			{
				buffer = other.buffer;
				allocation = other.allocation;
				size = other.size;

				other.buffer = VK_NULL_HANDLE;
				other.allocation = VK_NULL_HANDLE;
				other.size = 0;
			}

			VMABuffer& operator=(VMABuffer&& other) noexcept
			{
				if (this != &other)
				{
					free();

					buffer = other.buffer;
					allocation = other.allocation;
					size = other.size;

					other.buffer = VK_NULL_HANDLE;
					other.allocation = VK_NULL_HANDLE;
					other.size = 0;
				}

				return *this;
			}

			VMABuffer(const VMABuffer&) = delete;
			VMABuffer& operator=(const VMABuffer&) = delete;

			~VMABuffer()
			{
				free();
			}

			template <typename T>
			void CopyData(const T& inData)
			{
				auto allocator = VMAAllocator::Instance().Get();

				void* data;

				vmaMapMemory(allocator, allocation, &data);

				memcpy(data, inData, size);

				vmaUnmapMemory(allocator, allocation);
			}

			template <typename T>
			void CopyData(const T& inData, size_t elementSize)
			{
				auto allocator = VMAAllocator::Instance().Get();

				void* data;

				vmaMapMemory(allocator, allocation, &data);

				vk::DeviceSize offset = 0;
				for (const auto& d : inData)
				{
					memcpy((char*)data + offset, d, elementSize);
					offset += elementSize;
				}

				vmaUnmapMemory(allocator, allocation);
			}

			VkBuffer Get() const { return buffer; }

		private:
			size_t size;
			VkBuffer buffer = VK_NULL_HANDLE;
			VmaAllocation allocation = VK_NULL_HANDLE;

			void free() const
			{
				if (buffer != VK_NULL_HANDLE)
				{
					vmaDestroyBuffer(VMAAllocator::Instance().Get(), buffer, allocation);
				}
			}
	};
}