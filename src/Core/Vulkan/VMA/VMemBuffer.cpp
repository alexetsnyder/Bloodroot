#include "VMemBuffer.h"

#include <vulkan/vulkan_raii.hpp>

#include <iostream>

namespace Core::raii
{
	VMemBuffer::VMemBuffer()
		: size(0)
	{

	}

	VMemBuffer::VMemBuffer(VmaAllocator allocator, size_t size, VkBufferUsageFlags usage)
	{
		this->allocator = allocator;
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
			.usage = VMA_MEMORY_USAGE_CPU_TO_GPU,
		};

		auto result = vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Buffer!");
		}

		std::cout << "Created VMemBuffer.\n";
	}

	VMemBuffer::VMemBuffer(VMemBuffer&& other) noexcept
	{
		size = other.size;
		allocator = other.allocator;
		allocation = other.allocation;
		buffer = other.buffer;

		other.size = 0;
		other.allocation = VK_NULL_HANDLE;
		other.allocator = VK_NULL_HANDLE;
		other.buffer = VK_NULL_HANDLE;
	}

	VMemBuffer& VMemBuffer::operator=(VMemBuffer&& other) noexcept
	{
		if (this != &other)
		{
			if (buffer != VK_NULL_HANDLE)
			{
				std::cout << "Destroyed VMemBuffer.\n";
				vmaDestroyBuffer(allocator, buffer, allocation);
			}

			buffer = other.buffer;
			allocation = other.allocation;
			allocator = other.allocator;
			size = other.size;

			other.buffer = VK_NULL_HANDLE;
			other.allocation = VK_NULL_HANDLE;
			other.allocator = VK_NULL_HANDLE;
			other.size = 0;
		}

		return *this;
	}

	VMemBuffer::~VMemBuffer()
	{
		if (buffer != VK_NULL_HANDLE)
		{
			std::cout << "Destroyed VMemBuffer.\n";
			vmaDestroyBuffer(allocator, buffer, allocation);
		}
	}
};