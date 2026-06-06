#pragma once

#include "VMABuffer.h"
#include "VMAImage.h"

#include <Vulkan/vulkan_raii.hpp>

#include <memory>
#include <vector>

namespace Core::VK::CMD
{
	class CommandBufferManager
	{
		public:
			CommandBufferManager(std::nullptr_t);
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

			void CopyBuffer(VMA::VMABuffer&& srcBuffer, VMA::VMABuffer& dstBuffer, vk::BufferCopy bufferCopy);
			void CopyBufferToImage(VMA::VMABuffer&& buffer, VMA::VMAImage& image, uint32_t width, uint32_t height, uint32_t layerCount);

			void FlushCommandBuffer(const vk::raii::Queue& graphicsQueue);

		private:
			const vk::raii::Device* device = VK_NULL_HANDLE;
			vk::raii::CommandPool commandPool = VK_NULL_HANDLE;
			std::vector<vk::raii::CommandBuffer> commandBuffers;
			std::vector<VMA::VMABuffer> stagingBuffers;

			void checkForStart(uint32_t index = 0);
			void createCommandBuffers(uint32_t bufferCount);
	};
}