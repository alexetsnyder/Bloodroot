#pragma once

#include <VMA/vk_mem_alloc.h>

namespace Core::VMA
{
	class VMAVirtualAllocator
	{
		public:
			VMAVirtualAllocator();
			~VMAVirtualAllocator();

		private:
			VmaVirtualBlock block = VK_NULL_HANDLE;
	};
}