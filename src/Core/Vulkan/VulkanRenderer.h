#pragma once

#include "CommandBufferManager.h"
#include "GraphicsPipeline.h"
#include "IRenderer.h"
#include "Window.h"
#include "Vertex.h"
#include "VMAAllocator.h"
#include "VMABuffer.h"
#include "VMAImage.h"
#include "VMAVirtualBlock.h"
#include "VMAVirtualAllocation.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <Vulkan/vulkan_raii.hpp>

#include <memory>
#include <vector>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace Core::VK
{
	constexpr int MAX_FRAMES_IN_FLIGHT = 2;
	constexpr int TEXTURE_ARRAY_SIZE = 7;

	struct UniformBufferObject
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
	};

	struct Drawable
	{
		glm::i32vec3 chunkId;	
		uint32_t indexCount;
		glm::vec3 position;
		VMA::VMAVirtualAllocation allocation;

		Drawable(const glm::i32vec3& chunkId, uint32_t indexCount, glm::vec3 position, VMA::VMAVirtualAllocation allocation)
			: chunkId{ chunkId }, indexCount{ indexCount }, position{ position }, allocation{ std::move(allocation) }
		{

		}

		Drawable(Drawable&& other) noexcept
		{
			chunkId = other.chunkId;
			indexCount = other.indexCount;
			position = other.position;
			allocation = std::move(other.allocation);

			other.chunkId = glm::i32vec3{ 0 };
			other.indexCount = 0;
			other.position = glm::vec3{ 0.0f };
		}

		Drawable& operator=(Drawable&& other) noexcept
		{
			if (this != &other)
			{
				chunkId = other.chunkId;
				indexCount = other.indexCount;
				position = other.position;
				allocation = std::move(allocation);

				other.chunkId = glm::i32vec3{ 0 };
				other.indexCount = 0;
				other.position = glm::vec3(0.0f);
			}

			return *this;
		}

		Drawable(const Drawable&) = delete;
		Drawable& operator=(const Drawable&) = delete;
	};

	class VulkanRenderer : public IRenderer
	{
		public:
			VulkanRenderer(const Window& window, std::vector<const char*>&& requiredExtensions);
			~VulkanRenderer();

			void AddOpaqueMesh(const glm::i32vec3& chunkId,
							   uint32_t indexCount,
							   const glm::vec3& position,
							   const std::vector<Vertex>& verticies);

			void AddTransparentMesh(const glm::i32vec3& chunkId,
								    uint32_t indexCount,
								    const glm::vec3& position,
								    const std::vector<Vertex>& verticies);

			void SendIndexData(const std::vector<uint32_t>& indicies);

			void FlushCommandBuffer();
			
			void drawFrame(const Window& window, const glm::vec3& cameraPos, const glm::mat4& view);

			void waitIdle();

			void onResize(int width, int height) override;

		private:
			bool framebufferResized = false;
			vk::raii::Context context;
			vk::raii::Instance instance = nullptr;
			vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

			vk::raii::SurfaceKHR surface = nullptr;

			vk::raii::PhysicalDevice physicalDevice = nullptr;
			vk::raii::Device device = nullptr;

			uint32_t queueIndex = ~0;
			vk::raii::Queue graphicsQueue = nullptr;

			vk::raii::SwapchainKHR swapChain = nullptr;
			std::vector<vk::Image> swapChainImages;
			vk::SurfaceFormatKHR swapChainSurfaceFormat;
			vk::Extent2D swapChainExtent;
			std::vector<vk::raii::ImageView> swapChainImageViews;

			vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
			GraphicsPipeline opaqueGraphicsPipeline;
			GraphicsPipeline tGraphicsPipeline;

			CMD::CommandBufferManager commandBufferManager;
			CMD::CommandBufferManager transientCommandBufferManager;
			std::vector<VMA::VMABuffer> vmaStagingBuffers;

			uint32_t frameIndex = 0;
			std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
			std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
			std::vector<vk::raii::Fence> inFlightFences;

			VMA::VMAAllocator allocator;

			VMA::VMABuffer vertexBuffer;
			VMA::VMAVirtualBlock vertexBufferBlock;

			VMA::VMABuffer indexBuffer;

			std::vector<Drawable> drawables;
			std::vector<Drawable> tDrawables;

			std::vector<vk::raii::Buffer> uniformBuffers;
			std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
			std::vector<void*> uniformBuffersMapped;

			vk::raii::DescriptorPool descriptorPool = nullptr;
			std::vector<vk::raii::DescriptorSet> descriptorSets;

			uint32_t mipLevel;
			VMA::VMAImage textureImage = nullptr;
			vk::raii::ImageView textureImageView = nullptr;
			vk::raii::Sampler textureSampler = nullptr;

			VMA::VMAImage depthImage = nullptr;
			vk::raii::ImageView depthImageView = nullptr;

			std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

			void createInstance(const std::vector<const char*>& requiredExtensions);
			void setupDebugMessenger();
			static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
																  vk::DebugUtilsMessageTypeFlagsEXT type,
																  const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
																  void* pUserData);
			void createSurface(const Window& window);
			void pickPhysicalDevice();
			bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
			void createLogicalDevice();

			void createAllocator();

			void createSwapChain(int windowWidth, int windowHeight);
			vk::Extent2D chooseSwapChainExtent(vk::SurfaceCapabilitiesKHR const& capabilities, int windowWidth, int windowHeight);
			static uint32_t chooseSwapChainMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
			static vk::SurfaceFormatKHR chooseSwapChainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
			static vk::PresentModeKHR chooseSwapChainPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
			void createImageViews();

			void createDescriptorSetLayout();
			void createGraphicsPipelines();
			vk::Format findDepthFormat();
			vk::Format findSupportedFormat(const std::vector<vk::Format>& canidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
			
			void createCommandBufferManagers();
			void createDepthResources();

			uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
			vk::raii::ImageView createImageView(VMA::VMAImage& image, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t layerCount, vk::ImageViewType imageFormat);
			
			void createTextureImage();
			void generateMipmaps(VMA::VMAImage& image, vk::Format imageFormat, int32_t width, int32_t height, uint32_t mipLevels, uint32_t layerCount);
			void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory);
			void transitionImageLayout(const VMA::VMAImage& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount);
			void copyBufferToImage(const VMA::VMABuffer& buffer, VMA::VMAImage& image, uint32_t width, uint32_t height, uint32_t layerCount);

			void createTextureImageView();
			void createTextureSampler();

			void createVertexBuffer();
			void createIndexBuffer(const std::vector<uint32_t>& indicies);

			void AllocateToVertexBuffer(const glm::i32vec3& chunkId,
										uint32_t indexCount,
										const glm::vec3& position,
										const std::vector<Vertex>& verticies,
										std::vector<Drawable>& drawables);

			void copyBuffer(VMA::VMABuffer& srcBuffer, VMA::VMABuffer& dstBuffer, vk::BufferCopy bufferCopy);

			void createUniformBuffers();
			void createDescriptorPool();
			void createDescriptorSets();

			void createSyncObjects();

			void recreateSwapChain(const Window& window);
			void cleanUpSwapChain();
			void updateUniformBuffer(uint32_t currentImage, const glm::mat4& view);
			void recordCommandBuffer(uint32_t imageIndex, const glm::vec3& cameraPos);
			void transitionImageLayout(vk::Image image,
				vk::ImageLayout oldLayout,
				vk::ImageLayout newLayout,
				vk::AccessFlags2 srcAccessMask,
				vk::AccessFlags2 dstAccessMask,
				vk::PipelineStageFlags2 srcStageMask,
				vk::PipelineStageFlags2 dstStageMask,
				vk::ImageAspectFlags imageAspectFlags);
	};
}
