#pragma once

#include <Vulkan/vulkan_raii.hpp>

#include <vma/vk_mem_alloc.h>

namespace Core::VK::VMA
{
	class VMAAllocator
	{
		public:
			static VMAAllocator& Instance()
			{
				static VMAAllocator instance;

				if (instance.isFirstAccess)
				{
					instance.isFirstAccess = false;
					instance.init();
				}

				return instance;
			}

			VMAAllocator(VMAAllocator&& other) = delete;
			VMAAllocator& operator=(VMAAllocator&& other) = delete;

			VMAAllocator(const VMAAllocator&) = delete;
			VMAAllocator& operator=(const VMAAllocator&) = delete;

			VmaAllocator Get() const { return allocator; }

		private:
			bool isFirstAccess = true;
			VmaAllocator allocator = VK_NULL_HANDLE;

			VMAAllocator();
			~VMAAllocator();

			void init();
			void free();
	};
}