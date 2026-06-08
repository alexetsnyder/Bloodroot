#include "VulkanRenderer.h"

#include "Image.h"
#include "VulkanHandles.h"

#include "ShaderModule.h"

#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <math.h>
#include <map>
#include <ranges>

namespace Core::VK
{
	const std::vector<char const*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	VulkanRenderer::VulkanRenderer(const Window& window, std::vector<const char*>&& requiredExtensions)
	{
		std::cout << "Renderer created!\n";

		if (enableValidationLayers)
		{
			requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);
		}

		createInstance(requiredExtensions);
		setupDebugMessenger();

		createSurface(window);
		pickPhysicalDevice();
		createLogicalDevice();

		int width, height;
		window.getSize(width, height);

		createSwapChain(width, height);
		createImageViews();

		createDescriptorSetLayout();
		createGraphicsPipelines();

		createCommandBufferManagers();
		createDepthResources();

		createTexture();

		createVertexBuffer();

		createUniformBuffers();

		createDescriptorPool();
		createDescriptorSets();

		createSyncObjects();

		FlushCommandBuffer();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		std::cout << "Renderer destoyed!\n";
		cleanUpSwapChain();
	}

	void VulkanRenderer::AddOpaqueMesh(const glm::i32vec3& chunkId, uint32_t indexCount, const glm::vec3& position, const std::vector<Vertex>& verticies)
	{
		AllocateToVertexBuffer(chunkId, indexCount, position, verticies, this->drawables);
	}

	void VulkanRenderer::AddTransparentMesh(const glm::i32vec3& chunkId, uint32_t indexCount, const glm::vec3& position, const std::vector<Vertex>& verticies)
	{
		AllocateToVertexBuffer(chunkId, indexCount, position, verticies, this->tDrawables);
	}

	void VulkanRenderer::AddGuiMesh(uint32_t indexCount, const std::vector<Vertex>& verticies)
	{
		AllocateToVertexBuffer({ 0, 0, 0 }, indexCount, { 0, 0, 0 }, verticies, this->guiDrawable);
	}

	void VulkanRenderer::drawFrame(const Window& window, const glm::vec3& cameraPos, const glm::mat4& view)
	{
		auto& device = VulkanHandles::Instance().Device();

		auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
		if (fenceResult != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to wait for Fence!");
		}

		auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			recreateSwapChain(window);
			return;
		}

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		updateUniformBuffer(frameIndex, view);

		device.resetFences(*inFlightFences[frameIndex]);

		commandBufferManager.CommandBuffer(frameIndex).reset();
		recordCommandBuffer(imageIndex, cameraPos);

		vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		const vk::SubmitInfo submitInfo
		{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
			.pWaitDstStageMask = &waitDestinationStageMask,
			.commandBufferCount = 1,
			.pCommandBuffers = &*commandBufferManager.CommandBuffer(frameIndex), // commandBuffers[frameIndex],
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &*renderFinishedSemaphores[imageIndex]
		};

		graphicsQueue.submit(submitInfo, *inFlightFences[frameIndex]);

		const vk::PresentInfoKHR presentInfoKHR
		{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &*renderFinishedSemaphores[imageIndex],
			.swapchainCount = 1,
			.pSwapchains = &*swapChain,
			.pImageIndices = &imageIndex
		};

		result = graphicsQueue.presentKHR(presentInfoKHR);

		if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR || framebufferResized)
		{
			framebufferResized = false;
			recreateSwapChain(window);
		}
		else
		{
			assert(result == vk::Result::eSuccess);
		}

		frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::waitIdle()
	{
		VulkanHandles::Instance().Device().waitIdle();
	}

	void VulkanRenderer::onResize(int width, int height)
	{
		framebufferResized = true;
	}

	void VulkanRenderer::recreateSwapChain(const Window& window)
	{
		int width = 0, height = 0;
		window.getSize(width, height);
		while (width == 0 || height == 0)
		{
			window.getSize(width, height);
			glfwWaitEvents();
		}

		waitIdle();

		cleanUpSwapChain();

		createSwapChain(width, height);
		createImageViews();
		createDepthResources();
	}

	void VulkanRenderer::cleanUpSwapChain()
	{
		swapChainImageViews.clear();
		swapChain = nullptr;
	}

	void VulkanRenderer::updateUniformBuffer(uint32_t currentImage, const glm::mat4& view)
	{
		UniformBufferObject ubo{};
		ubo.view = view;
		ubo.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), 0.1f, 200.0f);

		//GLM designed for OpenGl where y coordinate is inverted.
		ubo.projection[1][1] *= -1;

		//Look at push constants
		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	void VulkanRenderer::recordCommandBuffer(uint32_t imageIndex, const glm::vec3& cameraPos)
	{
		const auto& commandBuffer = commandBufferManager.CommandBuffer(frameIndex);

		commandBuffer.begin({});

		transitionImageLayout(
			swapChainImages[imageIndex],
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eColorAttachmentOptimal,
			{},
			vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::ImageAspectFlagBits::eColor
		);

		transitionImageLayout(
			depthImage.Get(),
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eDepthAttachmentOptimal,
			vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
			vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
			vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
			vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
			vk::ImageAspectFlagBits::eDepth
		);

		vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
		vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

		vk::RenderingAttachmentInfo attachmentInfo =
		{
			.imageView = swapChainImageViews[imageIndex],
			.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.clearValue = clearColor
		};

		vk::RenderingAttachmentInfo depthAttachmentInfo
		{
			.imageView = depthImageView,
			.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eDontCare,
			.clearValue = clearDepth
		};

		vk::RenderingInfo renderingInfo =
		{
			.renderArea = {.offset = { 0, 0 }, .extent = swapChainExtent },
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &attachmentInfo,
			.pDepthAttachment = &depthAttachmentInfo
		};

		commandBuffer.beginRendering(renderingInfo);

		commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
		commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));

		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, opaqueGraphicsPipeline.PipelineLayout(), 0, *descriptorSets[frameIndex], nullptr);

		commandBuffer.bindIndexBuffer(indexBuffer.Get(), 0, vk::IndexType::eUint32);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *opaqueGraphicsPipeline.Pipeline());

		for (const auto& drawable : drawables)
		{
			commandBuffer.bindVertexBuffers(0, { vertexBuffer.Get() }, { drawable.allocation.Offset() });

			PushConstants pushConstants
			{
				glm::translate(glm::mat4(1.0f), drawable.position)
			};

			commandBuffer.pushConstants<PushConstants>(
				opaqueGraphicsPipeline.PipelineLayout(),
				vk::ShaderStageFlagBits::eVertex,
				0,
				pushConstants
			);

			commandBuffer.drawIndexed(drawable.indexCount, 1, 0, 0, 0);
		}

		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, tGraphicsPipeline.PipelineLayout(), 0, *descriptorSets[frameIndex], nullptr);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *tGraphicsPipeline.Pipeline());

		std::map<uint32_t, float> distances;
		std::vector<uint32_t> indicies(tDrawables.size());
		for (uint32_t i = 0; i < tDrawables.size(); i++)
		{
			float distance = glm::length(cameraPos - tDrawables[i].position);
			distances[i] = distance;
			indicies[i] = i;
		}

		std::sort(indicies.begin(), indicies.end(), 
			      [&distances](const uint32_t& i1, const uint32_t& i2) {
						return distances[i1] > distances[i2];
				  });

		for (const auto& index : indicies)
		{
			const auto& drawable = tDrawables[index];

			commandBuffer.bindVertexBuffers(0, { vertexBuffer.Get() }, { drawable.allocation.Offset() });

			PushConstants pushConstants
			{
				glm::translate(glm::mat4(1.0f), drawable.position)
			};

			commandBuffer.pushConstants<PushConstants>(
				tGraphicsPipeline.PipelineLayout(),
				vk::ShaderStageFlagBits::eVertex,
				0,
				pushConstants
			);

			commandBuffer.drawIndexed(drawable.indexCount, 1, 0, 0, 0);
		}

		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, guiGraphicsPipeline.PipelineLayout(), 0, *descriptorSets[frameIndex], nullptr);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *guiGraphicsPipeline.Pipeline());

		for (const auto& drawable : guiDrawable)
		{
			commandBuffer.bindVertexBuffers(0, { vertexBuffer.Get() }, { drawable.allocation.Offset() });

			commandBuffer.drawIndexed(drawable.indexCount, 1, 0, 0, 0);
		}

		commandBuffer.endRendering();

		transitionImageLayout(
			swapChainImages[imageIndex],
			vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			vk::AccessFlagBits2::eColorAttachmentWrite,
			{},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eBottomOfPipe,
			vk::ImageAspectFlagBits::eColor
		);

		commandBuffer.end();
	}

	void VulkanRenderer::transitionImageLayout(vk::Image image,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout,
		vk::AccessFlags2 srcAccessMask,
		vk::AccessFlags2 dstAccessMask,
		vk::PipelineStageFlags2 srcStageMask,
		vk::PipelineStageFlags2 dstStageMask,
		vk::ImageAspectFlags imageAspectFlags)
	{
		vk::ImageMemoryBarrier2 barrier =
		{
			.srcStageMask = srcStageMask,
			.srcAccessMask = srcAccessMask,
			.dstStageMask = dstStageMask,
			.dstAccessMask = dstAccessMask,
			.oldLayout = oldLayout,
			.newLayout = newLayout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = image,
			.subresourceRange =
			{
				.aspectMask = imageAspectFlags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		vk::DependencyInfo dependencyInfo =
		{
			.dependencyFlags = {},
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier
		};

		commandBufferManager.CommandBuffer(frameIndex).pipelineBarrier2(dependencyInfo);
	}

	void VulkanRenderer::createInstance(const std::vector<const char*>& requiredExtensions)
	{
		auto& handles = VulkanHandles::Instance();
		auto& context = handles.Context();
		auto& instance = handles.VKInstance();

		constexpr vk::ApplicationInfo appInfo
		{
			.pApplicationName = "Hello Triangle",
			.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
			.pEngineName = "No Engine",
			.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
			.apiVersion = vk::ApiVersion14

		};

		std::vector<char const*> requiredLayers;
		if (enableValidationLayers)
		{
			requiredLayers.assign(validationLayers.begin(), validationLayers.end());
		}

		auto layerProperties = context.enumerateInstanceLayerProperties();
		auto unsupportedLayerIt = std::ranges::find_if(requiredLayers,
			[&layerProperties](auto const& requiredLayer)
			{
				return std::ranges::none_of(layerProperties,
					[requiredLayer](auto const& layerProperty) {
						return strcmp(layerProperty.layerName, requiredLayer) == 0;
					});
			});

		if (unsupportedLayerIt != requiredLayers.end())
		{
			throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
		}

		auto extensionProperties = context.enumerateInstanceExtensionProperties();
		auto unsupportedProperyIt =
			std::ranges::find_if(requiredExtensions,
				[&extensionProperties](auto const& requiredExtension)
				{
					return std::ranges::none_of(extensionProperties,
						[&requiredExtension](auto const& extensionProperty)
						{
							return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
						});
				});

		if (unsupportedProperyIt != requiredExtensions.end())
		{
			throw std::runtime_error("Required extensions not supported: " + std::string(*unsupportedProperyIt));
		}

		vk::InstanceCreateInfo createInfo
		{
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
			.ppEnabledLayerNames = requiredLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
			.ppEnabledExtensionNames = requiredExtensions.data()
		};

		instance = vk::raii::Instance(context, createInfo);
	}

	void VulkanRenderer::setupDebugMessenger()
	{
		auto& instance = VulkanHandles::Instance().VKInstance();

		if (!enableValidationLayers)
		{
			return;
		}

		vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
		);
		vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
		);

		vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT
		{
			.messageSeverity = severityFlags,
			.messageType = messageTypeFlags,
			.pfnUserCallback = &debugCallback,
		};

		debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
	}

	VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanRenderer::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
		vk::DebugUtilsMessageTypeFlagsEXT type,
		const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError ||
			severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
		{
			std::cerr << "Validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
		}

		return vk::False;
	}

	void VulkanRenderer::createSurface(const Window& window)
	{
		auto& instance = VulkanHandles::Instance().VKInstance();

		VkSurfaceKHR _surface;
		if (window.createWindowSurface(*instance, _surface) != 0)
		{
			throw std::runtime_error("Failed to create window surface!");
		}

		surface = vk::raii::SurfaceKHR(instance, _surface);
	}

	void VulkanRenderer::pickPhysicalDevice()
	{
		auto& instance = VulkanHandles::Instance().VKInstance();
		auto& physicalDevice = VulkanHandles::Instance().PhysicalDevice();

		auto physicalDevices = instance.enumeratePhysicalDevices();
		auto const devIter = std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) { return isDeviceSuitable(physicalDevice); });

		if (devIter == physicalDevices.end())
		{
			throw std::runtime_error("Failed to find suitable GPU!");
		}

		physicalDevice = *devIter;
	}

	bool VulkanRenderer::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
	{
		bool supportVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

		auto queueFamilies = physicalDevice.getQueueFamilyProperties();
		bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp) { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

		auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
		bool supportsAllRequiredExtensions =
			std::ranges::all_of(requiredDeviceExtension,
				[&availableDeviceExtensions](auto const& requiredDeviceExtension)
				{
					return std::ranges::any_of(availableDeviceExtensions,
						[requiredDeviceExtension](auto const& availableDeviceExtension)
						{
							return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0;
						});
				});

		auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
			vk::PhysicalDeviceVulkan11Features,
			vk::PhysicalDeviceVulkan13Features,
			vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
		bool supportRequiredFeatures = features.template get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy &&
			features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
			features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
			features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
			features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

		return supportVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportRequiredFeatures;
	}

	void VulkanRenderer::createLogicalDevice()
	{
		auto& physicalDevice = VulkanHandles::Instance().PhysicalDevice();
		auto& device = VulkanHandles::Instance().Device();

		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

		for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
				physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
			{
				queueIndex = qfpIndex;
				break;
			}
		}

		if (queueIndex == ~0)
		{
			throw std::runtime_error("Could not find a queue for graphics and present -> terminating!");
		}

		vk::StructureChain<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain
		{
			{.features = {.samplerAnisotropy = true } },
			{.shaderDrawParameters = true },
			{.synchronization2 = true, .dynamicRendering = true },
			{.extendedDynamicState = true }
		};

		float queuePriority = 0.5f;
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo{ .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };
		vk::DeviceCreateInfo deviceCreateInfo
		{
			.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &deviceQueueCreateInfo,
			.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
			.ppEnabledExtensionNames = requiredDeviceExtension.data()
		};

		device = vk::raii::Device(physicalDevice, deviceCreateInfo);
		graphicsQueue = vk::raii::Queue(device, queueIndex, 0);
	}

	void VulkanRenderer::createSwapChain(int windowWidth, int windowHeight)
	{
		auto& physicalDevice = VulkanHandles::Instance().PhysicalDevice();
		auto& device = VulkanHandles::Instance().Device();

		vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
		swapChainExtent = chooseSwapChainExtent(surfaceCapabilities, windowWidth, windowHeight);
		uint32_t minImageCount = chooseSwapChainMinImageCount(surfaceCapabilities);

		std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
		swapChainSurfaceFormat = chooseSwapChainSurfaceFormat(availableFormats);

		std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
		vk::PresentModeKHR presentMode = chooseSwapChainPresentMode(availablePresentModes);

		vk::SwapchainCreateInfoKHR swapChainCreateInfo
		{
			.surface = *surface,
			.minImageCount = minImageCount,
			.imageFormat = swapChainSurfaceFormat.format,
			.imageColorSpace = swapChainSurfaceFormat.colorSpace,
			.imageExtent = swapChainExtent,
			.imageArrayLayers = 1,
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.preTransform = surfaceCapabilities.currentTransform,
			.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
			.presentMode = presentMode,
			.clipped = true
		};

		swapChain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
		swapChainImages = swapChain.getImages();
	}

	vk::Extent2D VulkanRenderer::chooseSwapChainExtent(vk::SurfaceCapabilitiesKHR const& capabilities, int windowWidth, int windowHeight)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}

		return
		{
			std::clamp<uint32_t>(windowWidth, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(windowHeight, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
	}

	uint32_t VulkanRenderer::chooseSwapChainMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
	{
		auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);

		if (surfaceCapabilities.maxImageCount > 0 && surfaceCapabilities.maxImageCount < minImageCount)
		{
			minImageCount = surfaceCapabilities.maxImageCount;
		}

		return minImageCount;
	}

	vk::SurfaceFormatKHR VulkanRenderer::chooseSwapChainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
	{
		assert(!availableFormats.empty());

		const auto formatIt = std::ranges::find_if(
			availableFormats,
			[](const auto& format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; }
		);

		return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
	}

	vk::PresentModeKHR VulkanRenderer::chooseSwapChainPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
	{
		assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));

		return std::ranges::any_of(availablePresentModes,
			[](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ?
			vk::PresentModeKHR::eMailbox :
			vk::PresentModeKHR::eFifo;
	}

	void VulkanRenderer::createImageViews()
	{
		assert(swapChainImageViews.empty());

		auto& device = VulkanHandles::Instance().Device();

		vk::ImageViewCreateInfo imageViewCreateInfo
		{
			.viewType = vk::ImageViewType::e2D,
			.format = swapChainSurfaceFormat.format,
			.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
		};

		for (auto& image : swapChainImages)
		{
			imageViewCreateInfo.image = image;
			swapChainImageViews.emplace_back(device, imageViewCreateInfo);
		}
	}

	void VulkanRenderer::createDescriptorSetLayout()
	{
		auto& device = VulkanHandles::Instance().Device();

		std::array bindings
		{
			vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),
			vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr),
			vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr),
		};

		vk::DescriptorSetLayoutCreateInfo layoutInfo{ .bindingCount = bindings.size(), .pBindings = bindings.data() };
		descriptorSetLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);
	}

	void VulkanRenderer::createGraphicsPipelines()
	{
		auto shaderModule = ShaderModule{ "Shaders/shader.spv" };

		vk::PipelineColorBlendAttachmentState opaqueColorBlendAttachment
		{
			.blendEnable = vk::False,
			.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		};

		vk::PipelineDepthStencilStateCreateInfo opaqueDepthStateCreateInfo
		{
			.depthTestEnable = vk::True,
			.depthWriteEnable = vk::True,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = vk::False,
			.stencilTestEnable = vk::False
		};

		opaqueGraphicsPipeline = GraphicsPipeline
		{ 
			shaderModule,
			swapChainSurfaceFormat.format,
			findDepthFormat(),
			descriptorSetLayout,
			opaqueColorBlendAttachment,
			opaqueDepthStateCreateInfo
		};

		vk::PipelineColorBlendAttachmentState tColorBlendAttachment
		{
			.blendEnable = vk::True,
			.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
			.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
			.colorBlendOp = vk::BlendOp::eAdd,
			.srcAlphaBlendFactor = vk::BlendFactor::eOne,
			.dstAlphaBlendFactor = vk::BlendFactor::eZero,
			.alphaBlendOp = vk::BlendOp::eAdd,
			.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		};

		vk::PipelineDepthStencilStateCreateInfo tDepthStateCreateInfo
		{
			.depthTestEnable = vk::True,
			.depthWriteEnable = vk::False,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = vk::False,
			.stencilTestEnable = vk::False
		};

		tGraphicsPipeline = GraphicsPipeline
		{
			shaderModule,
			swapChainSurfaceFormat.format,
			findDepthFormat(),
			descriptorSetLayout,
			tColorBlendAttachment,
			tDepthStateCreateInfo
		};

		auto guiShader = ShaderModule{ "Shaders/guiShader.spv" };

		vk::PipelineColorBlendAttachmentState guiColorBlendAttachment
		{
			.blendEnable = vk::False,
			.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		};

		vk::PipelineDepthStencilStateCreateInfo guiDepthStateCreateInfo
		{
			.depthTestEnable = vk::False,
			.depthWriteEnable = vk::False,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = vk::False,
			.stencilTestEnable = vk::False
		};

		guiGraphicsPipeline = GraphicsPipeline
		{
			guiShader,
			swapChainSurfaceFormat.format,
			findDepthFormat(),
			descriptorSetLayout,
			guiColorBlendAttachment,
			guiDepthStateCreateInfo
		};
	}

	vk::Format VulkanRenderer::findDepthFormat()
	{
		return findSupportedFormat(
			{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);
	}

	vk::Format VulkanRenderer::findSupportedFormat(const std::vector<vk::Format>& canidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
	{
		auto& physicalDevice = VulkanHandles::Instance().PhysicalDevice();

		for (const auto& format : canidates)
		{
			vk::FormatProperties props = physicalDevice.getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}

			if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error("Failed to find supported format!");
	}

	void VulkanRenderer::createCommandBufferManagers()
	{
		auto& device = VulkanHandles::Instance().Device();

		commandBufferManager = CMD::CommandBufferManager
		{
			device,
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			queueIndex,
			MAX_FRAMES_IN_FLIGHT
		};

		transientCommandBufferManager = CMD::CommandBufferManager
		{
			device,
			vk::CommandPoolCreateFlagBits::eTransient,
			queueIndex
		};
	}

	void VulkanRenderer::createDepthResources()
	{
		vk::Format depthFormat = findDepthFormat();

		depthImage = VMA::VMAImage
		{
			swapChainExtent.width,
			swapChainExtent.height,
			1,
			1,
			depthFormat,
			vk::ImageUsageFlagBits::eDepthStencilAttachment
		};

		depthImageView = createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1, 1, vk::ImageViewType::e2D);
	}

	uint32_t VulkanRenderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		auto& physicalDevice = VulkanHandles::Instance().PhysicalDevice();

		vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}

	vk::raii::ImageView VulkanRenderer::createImageView(VMA::VMAImage& image, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t layerCount, vk::ImageViewType imageFormat)
	{
		auto& device = VulkanHandles::Instance().Device();

		vk::ImageViewCreateInfo imageViewCreateInfo
		{
			.image = image.Get(),
			.viewType = imageFormat,
			.format = format,
			.subresourceRange = { aspectFlags, 0, mipLevels, 0, layerCount },
		};

		return vk::raii::ImageView(device, imageViewCreateInfo);
	}

	void VulkanRenderer::createTexture()
	{
		auto& physicalDevice = VulkanHandles::Instance().PhysicalDevice();
		auto& device = VulkanHandles::Instance().Device();

		Image images[TEXTURE_ARRAY_SIZE]
		{
			{ "Textures/Dirt.png" },
			{ "Textures/GrassSide.png" },
			{ "Textures/GrassTop.png" },
			{ "Textures/Stone.png" },
			{ "Textures/Bedrock.png" },
			{ "Textures/Sand.png" },
			{ "Textures/Water.png" }
		};

		texture = MAT::Texture
		{
			transientCommandBufferManager,
			images,
			TEXTURE_ARRAY_SIZE,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageViewType::e2DArray,
		};

		Image image{ "Textures/Crosshair.png" };

		crosshairTexture = MAT::Texture
		{
			transientCommandBufferManager,
			{ &image, 1 },
			1,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageViewType::e2D,
		};
	}

	void VulkanRenderer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory)
	{
		auto& device = VulkanHandles::Instance().Device();

		vk::BufferCreateInfo bufferInfo
		{
			.size = size,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive
		};

		buffer = vk::raii::Buffer(device, bufferInfo);

		vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();

		vk::MemoryAllocateInfo memoryAllocateInfo
		{
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties)
		};

		bufferMemory = vk::raii::DeviceMemory(device, memoryAllocateInfo);

		buffer.bindMemory(*bufferMemory, 0);
	}

	void VulkanRenderer::SendIndexData(const std::vector<uint32_t>& indicies)
	{
		createIndexBuffer(indicies);
	}

	void VulkanRenderer::FlushCommandBuffer()
	{
		transientCommandBufferManager.FlushCommandBuffer(graphicsQueue);
	}

	void VulkanRenderer::createVertexBuffer()
	{
		vk::DeviceSize bufferSize = 400 * 1024 * 1024;

		vertexBuffer = VMA::VMABuffer(
			bufferSize, 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_GPU_ONLY);

		vertexBufferBlock = VMA::VMAVirtualBlock
		{
			bufferSize,
		};
	}

	void VulkanRenderer::createIndexBuffer(const std::vector<uint32_t>& indicies)
	{
		vk::DeviceSize bufferSize = sizeof(indicies[0]) * indicies.size();

		VMA::VMABuffer stagingBuffer
		{
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY
		};

		stagingBuffer.CopyData(indicies.data());

		indexBuffer = VMA::VMABuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
			VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_GPU_ONLY);

		transientCommandBufferManager.CopyBuffer(std::move(stagingBuffer), indexBuffer, vk::BufferCopy(0, 0, bufferSize));
	}

	void VulkanRenderer::AllocateToVertexBuffer(const glm::i32vec3& chunkId,
												uint32_t indexCount,
												const glm::vec3& position,
												const std::vector<Vertex>& verticies,
												std::vector<Drawable>& drawables)
	{
		vk::DeviceSize bufferSize = sizeof(verticies[0]) * verticies.size();

		VMA::VMABuffer stagingBuffer
		{
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY
		};

		stagingBuffer.CopyData(verticies.data());

		Drawable drawable
		{
			chunkId,
			indexCount,
			position,
			VMA::VMAVirtualAllocation
			{
				vertexBufferBlock.Block(),
				bufferSize,
			},
		};

		transientCommandBufferManager.CopyBuffer(std::move(stagingBuffer), vertexBuffer, vk::BufferCopy(0, drawable.allocation.Offset(), bufferSize));

		drawables.emplace_back(std::move(drawable));
	}

	void VulkanRenderer::createUniformBuffers()
	{
		uniformBuffers.clear();
		uniformBuffersMemory.clear();
		uniformBuffersMapped.clear();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
			vk::raii::Buffer buffer({});
			vk::raii::DeviceMemory bufferMemory({});

			createBuffer(
				bufferSize,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible |
				vk::MemoryPropertyFlagBits::eHostCoherent,
				buffer,
				bufferMemory
			);

			uniformBuffers.emplace_back(std::move(buffer));
			uniformBuffersMemory.emplace_back(std::move(bufferMemory));
			uniformBuffersMapped.emplace_back(uniformBuffersMemory[i].mapMemory(0, bufferSize));
		}
	}

	void VulkanRenderer::createDescriptorPool()
	{
		auto& device = VulkanHandles::Instance().Device();

		std::array poolSize
		{
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT),
			vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, MAX_FRAMES_IN_FLIGHT),
			vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, MAX_FRAMES_IN_FLIGHT)
		};

		vk::DescriptorPoolCreateInfo poolInfo
		{
			.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			.maxSets = MAX_FRAMES_IN_FLIGHT,
			.poolSizeCount = poolSize.size(),
			.pPoolSizes = poolSize.data() };

		descriptorPool = vk::raii::DescriptorPool(device, poolInfo);
	}

	void VulkanRenderer::createDescriptorSets()
	{
		auto& device = VulkanHandles::Instance().Device();

		std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		vk::DescriptorSetAllocateInfo allocInfo
		{
			.descriptorPool = descriptorPool,
			.descriptorSetCount = static_cast<uint32_t>(layouts.size()),
			.pSetLayouts = layouts.data()
		};

		descriptorSets.clear();
		descriptorSets = device.allocateDescriptorSets(allocInfo);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::DescriptorBufferInfo bufferInfo
			{ 
				.buffer = uniformBuffers[i],
				.offset = 0, 
				.range = sizeof(UniformBufferObject)
			};
			
			vk::DescriptorImageInfo imageInfo
			{
				.sampler = texture.Sampler(),
				.imageView = texture.ImageView(),
				.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
			};

			vk::DescriptorImageInfo guiImageInfo
			{
				.sampler = crosshairTexture.Sampler(),
				.imageView = crosshairTexture.ImageView(),
				.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
			};

			std::array descriptorWrites
			{
				vk::WriteDescriptorSet
				{
					.dstSet = descriptorSets[i],
					.dstBinding = 0,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = vk::DescriptorType::eUniformBuffer,
					.pBufferInfo = &bufferInfo
				},
				vk::WriteDescriptorSet
				{
					.dstSet = descriptorSets[i],
					.dstBinding = 1,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = vk::DescriptorType::eCombinedImageSampler,
					.pImageInfo = &imageInfo
				},
				vk::WriteDescriptorSet
				{
					.dstSet = descriptorSets[i],
					.dstBinding = 2,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = vk::DescriptorType::eCombinedImageSampler,
					.pImageInfo = &guiImageInfo
				}
			};

			device.updateDescriptorSets(descriptorWrites, {});
		}
	}

	void VulkanRenderer::createSyncObjects()
	{
		auto& device = VulkanHandles::Instance().Device();

		assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			presentCompleteSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
			inFlightFences.emplace_back(device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
		}
	}
}