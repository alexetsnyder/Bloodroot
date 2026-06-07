#include "Texture.h"

#include "VMABuffer.h"
#include "VulkanHandles.h"

#include <ranges>

namespace Core::VK::MAT
{
	Texture::Texture(std::nullptr_t)
		: mipLevel{ 0 }
	{

	}

	Texture::Texture(CMD::CommandBufferManager& commandBufferManager,
					 std::span<const Image> images,
					 uint32_t layerCount,
					 vk::Format format,
					 vk::ImageViewType imageViewType)
	{
		const auto& physicalDevice = VulkanHandles::Instance().PhysicalDevice();

		mipLevel = images[0].getMipLevels();
		uint32_t width = images[0].Width();
		uint32_t height = images[0].Height();

		vk::DeviceSize totalSize = static_cast<vk::DeviceSize>(width) * height * 4 * layerCount;
		vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(width) * height * 4;

		auto tempStagingBuffer = VMA::VMABuffer
		{
			totalSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY
		};

		auto data = images | std::views::transform([](const Image& d) { return d.Data(); });

		tempStagingBuffer.CopyData(data, imageSize);

		vmaImage = VMA::VMAImage
		{
			width,
			height,
			mipLevel,
			layerCount,
			format,
			vk::ImageUsageFlagBits::eTransferSrc |
			vk::ImageUsageFlagBits::eTransferDst |
			vk::ImageUsageFlagBits::eSampled
		};

		transitionImageLayout(
			commandBufferManager.CommandBuffer(),
			vmaImage,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal,
			mipLevel,
			layerCount
		);

		commandBufferManager.CopyBufferToImage(std::move(tempStagingBuffer), vmaImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height), layerCount);

		generateMipmaps(
			commandBufferManager.CommandBuffer(),
			vmaImage,
			format,
			width,
			height,
			mipLevel,
			layerCount,
			physicalDevice.getFormatProperties(format)
		);

		createImageView(format, vk::ImageAspectFlagBits::eColor, mipLevel, layerCount, imageViewType);
		createSampler(); // device, physicalDevice.getProperties().limits.maxSamplerAnisotropy);
	}

	Texture::Texture(Texture&& other) noexcept
	{
		mipLevel = other.mipLevel;
		vmaImage = std::move(other.vmaImage);
		imageView = std::move(other.imageView);
		sampler = std::move(other.sampler);

		other.mipLevel = 0;
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			mipLevel = other.mipLevel;
			vmaImage = std::move(other.vmaImage);
			imageView = std::move(other.imageView);
			sampler = std::move(other.sampler);

			other.mipLevel = 0;
		}

		return *this;
	}

	Texture::~Texture()
	{

	}

	void Texture::generateMipmaps(const vk::raii::CommandBuffer& commandBuffer,
								  VMA::VMAImage& image,
								  vk::Format imageFormat,
								  int32_t width,
								  int32_t height,
								  uint32_t mipLevels,
								  uint32_t layerCount,
								  vk::FormatProperties physicalDeviceProperties)
	{
		if (!(physicalDeviceProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
		{
			throw std::runtime_error("Texture image format does not support linear blitting!");
		}

		vk::ImageMemoryBarrier barrier
		{
			.srcAccessMask = vk::AccessFlagBits::eTransferWrite,
			.dstAccessMask = vk::AccessFlagBits::eTransferRead,
			.oldLayout = vk::ImageLayout::eTransferDstOptimal,
			.newLayout = vk::ImageLayout::eTransferSrcOptimal,
			.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
			.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
			.image = image.Get()
		};

		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = width;
		int32_t mipHeight = height;

		for (uint32_t i = 1; i < mipLevels; i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barrier);

			vk::ArrayWrapper1D<vk::Offset3D, 2> offsets, dstOffsets;
			offsets[0] = vk::Offset3D(0, 0, 0);
			offsets[1] = vk::Offset3D(mipWidth, mipHeight, 1);
			dstOffsets[0] = vk::Offset3D(0, 0, 0);
			dstOffsets[1] = vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1);

			vk::ImageBlit blit
			{
				.srcSubresource = {},
				.srcOffsets = offsets,
				.dstSubresource = {},
				.dstOffsets = dstOffsets
			};
			blit.srcSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1, 0, layerCount);
			blit.dstSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, 0, layerCount);

			commandBuffer.blitImage(image.Get(), vk::ImageLayout::eTransferSrcOptimal, image.Get(), vk::ImageLayout::eTransferDstOptimal, { blit }, vk::Filter::eLinear);

			barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);

			if (mipWidth > 1)
			{
				mipWidth /= 2;
			}

			if (mipHeight > 1)
			{
				mipHeight /= 2;
			}
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);
	}

	void Texture::transitionImageLayout(const vk::raii::CommandBuffer& commandBuffer,
										const VMA::VMAImage& image,
										vk::ImageLayout oldLayout,
										vk::ImageLayout newLayout,
										uint32_t mipLevels,
										uint32_t layerCount)
	{
		vk::ImageMemoryBarrier barrier
		{
			.oldLayout = oldLayout,
			.newLayout = newLayout,
			.image = image.Get(),
			.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, layerCount }
		};

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
		{
			throw std::invalid_argument("Unsupported layout transitions!");
		}

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
	}

	void Texture::createImageView(vk::Format format,
								  vk::ImageAspectFlags aspectFlags,
								  uint32_t mipLevels,
								  uint32_t layerCount,
								  vk::ImageViewType imageViewType)
	{
		const auto& device = VulkanHandles::Instance().Device();

		vk::ImageViewCreateInfo imageViewCreateInfo
		{
			.image = vmaImage.Get(),
			.viewType = imageViewType,
			.format = format,
			.subresourceRange = { aspectFlags, 0, mipLevels, 0, layerCount },
		};

		imageView = vk::raii::ImageView(device, imageViewCreateInfo);
	}

	void Texture::createSampler()
	{
		const auto& physicalDevice = VulkanHandles::Instance().PhysicalDevice();
		const auto& device = VulkanHandles::Instance().Device();

		vk::SamplerCreateInfo samplerInfo
		{
			.magFilter = vk::Filter::eNearest,
			.minFilter = vk::Filter::eNearest,
			.mipmapMode = vk::SamplerMipmapMode::eLinear,
			.addressModeU = vk::SamplerAddressMode::eClampToEdge,
			.addressModeV = vk::SamplerAddressMode::eClampToEdge,
			.addressModeW = vk::SamplerAddressMode::eClampToEdge,
			.mipLodBias = 0.0f,
			.anisotropyEnable = vk::True,
			.maxAnisotropy = physicalDevice.getProperties().limits.maxSamplerAnisotropy,
			.compareEnable = vk::False,
			.compareOp = vk::CompareOp::eAlways,
			.minLod = 0.0f,
			.maxLod = vk::LodClampNone
		};

		sampler = vk::raii::Sampler(device, samplerInfo);
	}
}