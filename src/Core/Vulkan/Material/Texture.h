#pragma once

#include "CommandBufferManager.h"
#include "Image.h"
#include "VMAAllocator.h"
#include "VMAImage.h"

#include <Vulkan/vulkan_raii.hpp>

#include <cstddef>
#include <span>

namespace Core::VK::MAT
{
	class Texture
	{
		public:
			Texture(std::nullptr_t);

			Texture(CMD::CommandBufferManager& commandBufferManager,
					std::span<const Image> images,
					uint32_t layerCount,
					vk::Format format,
					vk::ImageViewType imageViewType);

			Texture(Texture&& other) noexcept;
			Texture& operator=(Texture&& other) noexcept;

			Texture(const Texture&) = delete;
			Texture& operator=(const Texture&) = delete;

			~Texture();

			const vk::raii::Sampler& Sampler() const { return sampler; }
			const vk::raii::ImageView& ImageView() const { return imageView; }
		
		private:
			uint32_t mipLevel;
			VMA::VMAImage vmaImage = nullptr;
			vk::raii::ImageView imageView = nullptr;
			vk::raii::Sampler sampler = nullptr;

			void generateMipmaps(const vk::raii::CommandBuffer& commandBuffer,
								 VMA::VMAImage& image,
								 vk::Format imageFormat,
								 int32_t width,
								 int32_t height,
								 uint32_t mipLevels,
								 uint32_t layerCount,
								 vk::FormatProperties physicalDeviceProperties);
			void transitionImageLayout(const vk::raii::CommandBuffer& commandBuffer, 
									   const VMA::VMAImage& image,
									   vk::ImageLayout oldLayout,
									   vk::ImageLayout newLayout,
									   uint32_t mipLevels,
									   uint32_t layerCount);
			void createImageView(vk::Format format,
								 vk::ImageAspectFlags aspectFlags,
								 uint32_t mipLevels,
								 uint32_t layerCount,
								 vk::ImageViewType imageViewType);
			void createSampler();
	};
}