#include "VMemAlloc.h"

#include <iostream>

namespace Core::raii
{
	VMemAlloc::VMemAlloc()
	{
		allocator = nullptr;
	}

	VMemAlloc::VMemAlloc(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device, const vk::raii::Instance& instance)
	{
		VmaVulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
		vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocatorCreateInfo
		{
			.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
			.physicalDevice = *physicalDevice,
			.device = *device,	
			.pVulkanFunctions = &vulkanFunctions,
			.instance = *instance,
			.vulkanApiVersion = VK_API_VERSION_1_0,
			
		};

		auto result = vmaCreateAllocator(&allocatorCreateInfo, &allocator);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Allocator!");
		}

		std::cout << "Created VMA Allocator.\n";
	}

	VMemAlloc::VMemAlloc(VMemAlloc&& other) noexcept
	{
		allocator = other.allocator;

		other.allocator = VK_NULL_HANDLE;
	}

	VMemAlloc& VMemAlloc::operator=(VMemAlloc&& other) noexcept
	{
		if (this != &other)
		{
			if (allocator != VK_NULL_HANDLE)
			{
				std::cout << "Destroying VMA Allocator.\n";
				vmaDestroyAllocator(allocator);
			}

			allocator = other.allocator;
			other.allocator = VK_NULL_HANDLE;
		}

		return *this;
	}

	VMemAlloc::~VMemAlloc()
	{
		if (allocator != VK_NULL_HANDLE)
		{
			std::cout << "Destroying VMA Allocator.\n";
			vmaDestroyAllocator(allocator);
		}
	}
}