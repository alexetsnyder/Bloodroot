#pragma once

#include <vma/vk_mem_alloc.h>

namespace Core::raii
{
	class VMemBuffer
	{
		public:
			VMemBuffer();
			VMemBuffer(VmaAllocator allocator, size_t size, VkBufferUsageFlags usage);
			VMemBuffer(VMemBuffer&& other) noexcept;
			VMemBuffer& operator=(VMemBuffer&& other) noexcept;
			~VMemBuffer();

			VkBuffer Buffer() const { return buffer; }

			template <typename T>
			void SendData(T inData)
			{
				void* data;
				vmaMapMemory(allocator, allocation, &data);

				memcpy(data, inData, size);

				vmaUnmapMemory(allocator, allocation);
			}

			VMemBuffer(const VMemBuffer&) = delete;
			VMemBuffer& operator=(const VMemBuffer&) = delete;

		private:
			size_t size;
			VkBuffer buffer = VK_NULL_HANDLE;
			VmaAllocation allocation = VK_NULL_HANDLE;
			VmaAllocator allocator = VK_NULL_HANDLE;
	};
}