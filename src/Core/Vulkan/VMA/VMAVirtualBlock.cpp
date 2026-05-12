#include "VMAVirtualBlock.h"

#include <iostream>

namespace Core::VMA
{
	VMAVirtualBlock::VMAVirtualBlock(size_t size)
	{
		VmaVirtualBlockCreateInfo blockCreateInfo
		{
			.size = size, // 1024 * 1024,
		};

		VkResult result = vmaCreateVirtualBlock(&blockCreateInfo, &block);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to Allocate Virtual Block!");
		}

		std::cout << "Creating Virtual Block!\n";
	}

	VMAVirtualBlock::VMAVirtualBlock(VMAVirtualBlock&& other) noexcept
	{
		block = other.block;

		other.block = VK_NULL_HANDLE;
	}

	VMAVirtualBlock& VMAVirtualBlock::operator=(VMAVirtualBlock&& other) noexcept
	{
		if (this != &other)
		{
			free();

			block = other.block;

			other.block = VK_NULL_HANDLE;
		}

		return *this;
	}

	VMAVirtualBlock::~VMAVirtualBlock()
	{
		free();
	}

	void VMAVirtualBlock::free()
	{
		if (block != VK_NULL_HANDLE)
		{
			std::cout << "Destroying Virtual Block!\n";
			vmaDestroyVirtualBlock(block);
		}
	}
}