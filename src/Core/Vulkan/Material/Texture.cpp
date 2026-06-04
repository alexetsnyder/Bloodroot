#include "Texture.h"

#include "VMABuffer.h"

namespace Core::VK::MAT
{
	Texture::Texture()
	{

	}

	Texture::Texture(const vk::raii::Device& device,
					 const vk::raii::CommandBuffer& commandBuffer,
					 const Image& image, vk::Format format,
					 vk::ImageUsageFlags usage,
					 vk::MemoryPropertyFlags properties,
					 float maxSamplerAnisotropy)
		: vmaImage{ image.Width(), image.Height(), image.getMipLevels(), 1, format, usage, properties }
	{
		auto width = image.Width();
		auto height = image.Height();

		vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(width) * height * 4;

		/*VMA::VMABuffer stagingBuffer
		{
			vmaImage.Allocation(),
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY
		};

		stagingBuffer.CopyData(image.Data());*/


	}

	Texture::Texture(const vk::raii::Device & device,
					 const vk::raii::CommandBuffer & commandBuffer,
					 const std::vector<Image>&images,
					 uint32_t layerCount,
					 vk::Format format,
					 vk::ImageUsageFlags usage, 
					 vk::MemoryPropertyFlags properties,
					 float maxSamplerAnisotropy)
		: vmaImage{ images[0].Width(), images[0].Height(), images[0].getMipLevels(), 1, format, usage, properties}
	{

	}

	Texture::~Texture()
	{

	}
}