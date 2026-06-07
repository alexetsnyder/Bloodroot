#include "VMAAllocator.h"

#include "VulkanHandles.h"

#include <iostream>

namespace Core::VK::VMA
{
	VMAAllocator::VMAAllocator()
	{

	}

	/*VMAAllocator::VMAAllocator(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device, const vk::raii::Instance& instance)
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
	}*/

	/*VMAAllocator::VMAAllocator(VMAAllocator&& other) noexcept
	{
		allocator = other.allocator;

		other.allocator = VK_NULL_HANDLE;
	}

	VMAAllocator& VMAAllocator::operator=(VMAAllocator&& other) noexcept
	{
		if (this != &other)
		{
			free();

			allocator = other.allocator;
			other.allocator = VK_NULL_HANDLE;
		}

		return *this;
	}*/

	VMAAllocator::~VMAAllocator()
	{
		free();
	}

	void VMA::VMAAllocator::init()
	{
		auto& handles = VulkanHandles::Instance();
		auto& physicalDevice = handles.PhysicalDevice();
		auto& device = handles.Device();
		auto& instance = handles.VKInstance();

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

	void VMAAllocator::free()
	{
		if (allocator != VK_NULL_HANDLE)
		{
			std::cout << "Destroying VMA Allocator.\n";
			vmaDestroyAllocator(allocator);
		}
	}
}