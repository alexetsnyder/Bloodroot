#pragma once

#include <VMA/vk_mem_alloc.h>

namespace Core::VMA
{
	class VMAVirtualBlock
	{
		public:
			VMAVirtualBlock() {}
			VMAVirtualBlock(size_t size);
			VMAVirtualBlock(VMAVirtualBlock&& other) noexcept;
			VMAVirtualBlock& operator=(VMAVirtualBlock&& other) noexcept;

			VMAVirtualBlock(const VMAVirtualBlock&) = delete;
			VMAVirtualBlock& operator=(const VMAVirtualBlock&) = delete;

			~VMAVirtualBlock();

			VmaVirtualBlock Block() { return block; }

		private:
			VmaVirtualBlock block = VK_NULL_HANDLE;

			void free();
	};
}