#include "VMAVirtualAllocator.h"

namespace Core::VMA
{
	VMAVirtualAllocator::VMAVirtualAllocator()
	{
		VmaVirtualBlockCreateInfo blockCreateInfo
		{
			.size = 1024 * 1024,
		};

		VkResult result = vmaCreateVirtualBlock(&blockCreateInfo, &block);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to Allocate Virtual Block!");
		}

		std::cout << "Creating Virtual Block!";
	}

	VMAVirtualAllocator::~VMAVirtualAllocator()
	{
		if (block != VK_NULL_HANDLE)
		{
			std::cout << "Destroying Virtual Block!";
			vmaDestroyVirtualBlock(block);
		}
	}
}