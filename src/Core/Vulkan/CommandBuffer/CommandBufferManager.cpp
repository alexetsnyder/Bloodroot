#include "CommandBufferManager.h"

#include <assert.h>
#include <memory>

namespace Core::VK::CMD
{
	CommandBufferManager::CommandBufferManager()
	{

	}

	CommandBufferManager::CommandBufferManager(const vk::raii::Device& device,
											   vk::CommandPoolCreateFlags flags,
											   uint32_t queueIndex, 
											   uint32_t bufferCount)
	{
		this->device = &device;

		vk::CommandPoolCreateInfo poolInfo
		{
			.flags = flags,
			.queueFamilyIndex = queueIndex
		};

		commandPool = vk::raii::CommandPool(device, poolInfo);

		if (bufferCount > 0)
		{
			createCommandBuffers(bufferCount);
		}	
	}

	CommandBufferManager::CommandBufferManager(CommandBufferManager&& other) noexcept
	{
		device = other.device;
		commandPool = std::move(other.commandPool);
		commandBuffers = std::move(other.commandBuffers);
	}

	CommandBufferManager& CommandBufferManager::operator=(CommandBufferManager&& other) noexcept
	{
		if (this != &other)
		{
			device = other.device;
			commandPool = std::move(other.commandPool);
			commandBuffers = std::move(other.commandBuffers);
		}

		return *this;
	}

	CommandBufferManager::~CommandBufferManager()
	{
		device = VK_NULL_HANDLE;
	}

	vk::raii::CommandBuffer& CommandBufferManager::CommandBuffer(uint32_t index)
	{
		if (commandBuffers.empty())
		{
			createCommandBuffers(1);

			commandBuffers[index].begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		}

		return commandBuffers[index];
	}

	void CommandBufferManager::FlushCommandBuffer(const vk::raii::Queue& graphicsQueue)
	{
		assert(!commandBuffers.empty() && commandBuffers.size() == 1);

		commandBuffers[0].end();

		vk::SubmitInfo submitInfo
		{ 
			.commandBufferCount = 1,
			.pCommandBuffers = &*commandBuffers[0] 
		};
		graphicsQueue.submit(submitInfo, nullptr);
		graphicsQueue.waitIdle();
		
		commandBuffers.clear();
	}

	void CommandBufferManager::createCommandBuffers(uint32_t bufferCount)
	{
		vk::CommandBufferAllocateInfo allocInfo
		{
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = bufferCount
		};

		commandBuffers = vk::raii::CommandBuffers(*this->device, allocInfo);
	}
}