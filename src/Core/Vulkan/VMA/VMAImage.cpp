#include "VMAImage.h"

#include "VMABuffer.h"

namespace Core::VK::VMA
{
	VMAImage::VMAImage(std::nullptr_t)
	{

	}

	VMAImage::VMAImage(uint32_t width,
					   uint32_t height,
					   uint32_t mipLevels,
					   uint32_t layerCount,
					   vk::Format format,
					   vk::ImageUsageFlags usage)
	{
		vk::ImageCreateInfo imageInfo
		{
			.imageType = vk::ImageType::e2D,
			.format = format,
			.extent = { width, height, 1 },
			.mipLevels = mipLevels,
			.arrayLayers = layerCount,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive,
		};

		VmaAllocationCreateInfo allocInfo
		{
			.usage = VMA_MEMORY_USAGE_GPU_ONLY,
			.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};

		vmaCreateImage(VMAAllocator::Instance().Get(), imageInfo, &allocInfo, &vkImage, &allocation, nullptr);
	}

	VMAImage::VMAImage(VMAImage&& other) noexcept
	{
		vkImage = other.vkImage;
		allocation = other.allocation;

		other.vkImage = VK_NULL_HANDLE;
		other.allocation = VK_NULL_HANDLE;
	}

	VMAImage& VMAImage::operator=(VMAImage&& other) noexcept
	{
		if (this != &other)
		{
			free();

			vkImage = other.vkImage;
			allocation = other.allocation;

			other.vkImage = VK_NULL_HANDLE;
			other.allocation = VK_NULL_HANDLE;
		}

		return *this;
	}

	VMAImage::~VMAImage()
	{
		free();
	}

	void VMAImage::free() const
	{
		if (vkImage != VK_NULL_HANDLE)
		{
			vmaDestroyImage(VMAAllocator::Instance().Get(), vkImage, allocation);
		}
	}
}