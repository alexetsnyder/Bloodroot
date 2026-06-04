#pragma once

#include <Vulkan/vulkan_raii.hpp>

#include <vector>

namespace Core::VK::CMD
{
	class CommandBufferManager
	{
		public:
			CommandBufferManager();
			CommandBufferManager(const vk::raii::Device& device, uint32_t queueIndex);

			CommandBufferManager(CommandBufferManager&& other) noexcept;
			CommandBufferManager& operator=(CommandBufferManager&& other) noexcept;

			CommandBufferManager(const CommandBufferManager&) = delete;
			CommandBufferManager& operator=(const CommandBufferManager&) = delete;

			~CommandBufferManager();

			vk::raii::CommandBuffer& CommandBuffer(uint32_t index = 0) { return commandBuffers[index]; }

			void CreateCommandBuffers(const vk::raii::Device& device, uint32_t bufferCount);

			std::unique_ptr<vk::raii::CommandBuffer> BeginSingleTimeCommands(const vk::raii::Device& device);
			void EndSingleTimeCommands(const vk::raii::Queue graphicsQueue, const vk::raii::CommandBuffer& commandBuffer);

		private:
			vk::raii::CommandPool commandPool = nullptr;
			std::vector<vk::raii::CommandBuffer> commandBuffers;
	};
}