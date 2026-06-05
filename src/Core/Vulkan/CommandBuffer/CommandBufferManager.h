#pragma once

#include <Vulkan/vulkan_raii.hpp>

#include <memory>
#include <vector>

namespace Core::VK::CMD
{
	class CommandBufferManager
	{
		public:
			CommandBufferManager();
			CommandBufferManager(const vk::raii::Device& device,
								 vk::CommandPoolCreateFlags flags,
								 uint32_t queueIndex,
								 uint32_t bufferCount = 0);

			CommandBufferManager(CommandBufferManager&& other) noexcept;
			CommandBufferManager& operator=(CommandBufferManager&& other) noexcept;

			CommandBufferManager(const CommandBufferManager&) = delete;
			CommandBufferManager& operator=(const CommandBufferManager&) = delete;

			~CommandBufferManager();

			vk::raii::CommandBuffer& CommandBuffer(uint32_t index = 0);

			void FlushCommandBuffer(const vk::raii::Queue& graphicsQueue);

		private:
			const vk::raii::Device* device = VK_NULL_HANDLE;
			vk::raii::CommandPool commandPool = VK_NULL_HANDLE;
			std::vector<vk::raii::CommandBuffer> commandBuffers;

			void createCommandBuffers(uint32_t bufferCount);
	};
}