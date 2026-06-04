#include "CommandBufferManager.h"

#include <memory>

namespace Core::VK::CMD
{
	CommandBufferManager::CommandBufferManager()
	{

	}

	CommandBufferManager::CommandBufferManager(const vk::raii::Device& device, uint32_t queueIndex)
	{
		vk::CommandPoolCreateInfo poolInfo
		{
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = queueIndex
		};

		commandPool = vk::raii::CommandPool(device, poolInfo);
	}

	CommandBufferManager::CommandBufferManager(CommandBufferManager&& other) noexcept
	{
		commandPool = std::move(other.commandPool);
		commandBuffers = std::move(other.commandBuffers);
	}

	CommandBufferManager& CommandBufferManager::operator=(CommandBufferManager&& other) noexcept
	{
		if (this != &other)
		{
			commandPool = std::move(other.commandPool);
			commandBuffers = std::move(other.commandBuffers);
		}

		return *this;
	}

	CommandBufferManager::~CommandBufferManager()
	{

	}

	void CommandBufferManager::CreateCommandBuffers(const vk::raii::Device& device, uint32_t bufferCount)
	{
		vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = bufferCount };
		commandBuffers = vk::raii::CommandBuffers(device, allocInfo);
	}

	std::unique_ptr<vk::raii::CommandBuffer> CommandBufferManager::BeginSingleTimeCommands(const vk::raii::Device& device)
	{
		//TODO: Could create seperate CommandBool with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
		vk::CommandBufferAllocateInfo allocInfo
		{
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};
		std::unique_ptr<vk::raii::CommandBuffer> commandBuffer = std::make_unique<vk::raii::CommandBuffer>(std::move(device.allocateCommandBuffers(allocInfo).front()));

		vk::CommandBufferBeginInfo beginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
		commandBuffer->begin(beginInfo);

		return commandBuffer;
	}

	void CommandBufferManager::EndSingleTimeCommands(const vk::raii::Queue graphicsQueue, const vk::raii::CommandBuffer& commandBuffer)
	{
		commandBuffer.end();

		vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
		graphicsQueue.submit(submitInfo, nullptr);
		graphicsQueue.waitIdle();
	}
}