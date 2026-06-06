#include "VMAImage.h"

#include "VMABuffer.h"

namespace Core::VK::VMA
{
	VMAImage::VMAImage(std::nullptr_t)
	{

	}

	VMAImage::VMAImage(VmaAllocator allocator,
					   uint32_t width,
					   uint32_t height,
					   uint32_t mipLevels,
					   uint32_t layerCount,
					   vk::Format format,
					   vk::ImageUsageFlags usage,
					   vk::MemoryPropertyFlags properties)
	{
		//vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(width) * height * 4;

		/*VMABuffer stagingBuffer
		{
			allocator,
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY
		};

		stagingBuffer.CopyData(image.Data());*/

		this->allocator = allocator;

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
			.sharingMode = vk::SharingMode::eExclusive
		};

		VmaAllocationCreateInfo allocInfo
		{
			.usage = VMA_MEMORY_USAGE_GPU_ONLY,
			.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};

		vmaCreateImage(allocator, imageInfo, &allocInfo, &vkImage, &allocation, nullptr);

		//vk::ImageViewCreateInfo imageViewCreateInfo
		//{
		//	.image = vkImage,
		//	.viewType = vk::ImageViewType::e2D, // vk::ImageViewType::e2DArray,
		//	.format = format,
		//	.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, 1 },
		//};

		//vkImageView = vk::raii::ImageView(*device, imageViewCreateInfo);
		//vkCreateImageView(**device, imageViewCreateInfo, nullptr, &vkImageView);
	}

	/*VMAImage::VMAImage(std::shared_ptr<vk::raii::Device> device,
					   const std::vector<Image>& images,
					   uint32_t layerCount,
					   vk::Format format,
					   vk::ImageUsageFlags usage,
					   vk::MemoryPropertyFlags properties)
	{
		auto width = images[0].Width();
		auto height = images[0].Height();
		auto mipLevels = images[0].getMipLevels();

		vk::DeviceSize totalSize = static_cast<vk::DeviceSize>(width) * height * 4 * layerCount;
		vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(width) * height * 4;

		VMABuffer stagingBuffer
		{
			allocator,
			totalSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY
		};

		auto data = images | std::views::transform([](const Image& d) { return d.Data(); });

		stagingBuffer.CopyData(data, imageSize);

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
			.sharingMode = vk::SharingMode::eExclusive
		};

		VmaAllocationCreateInfo allocInfo
		{
			.usage = VMA_MEMORY_USAGE_GPU_ONLY,
			.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};

		vmaCreateImage(allocator, imageInfo, &allocInfo, &vkImage, &allocation, nullptr);

		vk::ImageViewCreateInfo imageViewCreateInfo
		{
			.image = vkImage,
			.viewType = vk::ImageViewType::e2DArray,
			.format = format,
			.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, layerCount },
		};

		vkImageView = vk::raii::ImageView(*device, imageViewCreateInfo);
	}*/

	VMAImage::VMAImage(VMAImage&& other) noexcept
	{
		vkImage = other.vkImage;
		allocation = other.allocation;
		allocator = other.allocator;

		other.vkImage = VK_NULL_HANDLE;
		other.allocation = VK_NULL_HANDLE;
		other.allocator = VK_NULL_HANDLE;
	}

	VMAImage& VMAImage::operator=(VMAImage&& other) noexcept
	{
		if (this != &other)
		{
			free();

			vkImage = other.vkImage;
			allocation = other.allocation;
			allocator = other.allocator;

			other.vkImage = VK_NULL_HANDLE;
			other.allocation = VK_NULL_HANDLE;
			other.allocator = VK_NULL_HANDLE;
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
			vmaDestroyImage(allocator, vkImage, allocation);
		}
	}
}