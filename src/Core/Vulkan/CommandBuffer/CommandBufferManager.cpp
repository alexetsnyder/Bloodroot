#include "CommandBufferManager.h"

#include <assert.h>
#include <memory>

namespace Core::VK::CMD
{
	CommandBufferManager::CommandBufferManager(std::nullptr_t)
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
		checkForStart(index);

		return commandBuffers[index];
	}

	void CommandBufferManager::CopyBuffer(VMA::VMABuffer&& srcBuffer, VMA::VMABuffer& dstBuffer, vk::BufferCopy bufferCopy)
	{
		checkForStart();

		assert(!commandBuffers.empty() && commandBuffers.size() == 1);

		stagingBuffers.emplace_back(std::move(srcBuffer));

		const auto& stagingBuffer = stagingBuffers.back();

		commandBuffers[0].copyBuffer(stagingBuffer.Buffer(), dstBuffer.Buffer(), bufferCopy);
	}

	void CommandBufferManager::CopyBufferToImage(VMA::VMABuffer&& buffer, VMA::VMAImage& image, uint32_t width, uint32_t height, uint32_t layerCount)
	{
		checkForStart();

		assert(!commandBuffers.empty() && commandBuffers.size() == 1);

		vk::BufferImageCopy region
		{
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, layerCount },
			.imageOffset = { 0, 0, 0 },
			.imageExtent = { width, height, 1 }
		};

		stagingBuffers.emplace_back(std::move(buffer));

		const auto& stagingBuffer = stagingBuffers.back();

		commandBuffers[0].copyBufferToImage(stagingBuffer.Buffer(), image.Image(), vk::ImageLayout::eTransferDstOptimal, {region});
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
		stagingBuffers.clear();
	}

	void CommandBufferManager::checkForStart(uint32_t index)
	{
		if (commandBuffers.empty())
		{
			createCommandBuffers(1);

			commandBuffers[index].begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		}

		assert(commandBuffers.size() > index);
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