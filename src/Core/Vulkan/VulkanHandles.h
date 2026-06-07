#pragma once

#include <Vulkan/vulkan_raii.hpp>

namespace Core::VK
{
	class VulkanHandles
	{
		public:
			static VulkanHandles& Instance()
			{
				static VulkanHandles instance;
				return instance;
			}

			vk::raii::Context& Context() { return context; }
			vk::raii::Instance& VKInstance() { return vkInstance; }
			vk::raii::PhysicalDevice& PhysicalDevice() { return physicalDevice; }
			vk::raii::Device& Device() { return device; }

		private:
			vk::raii::Context context;
			vk::raii::Instance vkInstance = nullptr;
			vk::raii::PhysicalDevice physicalDevice = nullptr;
			vk::raii::Device device = nullptr;

			VulkanHandles() {};
			~VulkanHandles() {};
	};
}