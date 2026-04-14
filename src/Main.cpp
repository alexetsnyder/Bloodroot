#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_raii.hpp>

#include <assert.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<char const*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

static std::vector<char> readFile(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + fileName + "!\n");
	}

	std::vector<char> buffer(file.tellg());

	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	file.close();

	return buffer;
}

class HelloTirangleApp
{
	public:
		void run()
		{
			initWindow();
			initVulkan();
			mainLoop();
			cleanUp();
		}

	private:
		bool framebufferResized = false;
		GLFWwindow* window = nullptr;

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

		vk::raii::PipelineLayout pipelineLayout = nullptr;
		vk::raii::Pipeline graphicsPipeline = nullptr;

		vk::raii::CommandPool commandPool = nullptr;
		std::vector<vk::raii::CommandBuffer> commandBuffers;

		uint32_t frameIndex = 0;
		std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
		std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
		std::vector<vk::raii::Fence> inFlightFences;

		std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

		void initWindow()
		{
			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Triangle!", nullptr, nullptr);

			glfwSetWindowUserPointer(window, this);
			glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
		}

		static void framebufferResizedCallback(GLFWwindow* window, int width, int height)
		{
			auto app = reinterpret_cast<HelloTirangleApp*>(glfwGetWindowUserPointer(window));
			app->framebufferResized = true;
		}

		void initVulkan()
		{
			createInstance();
			setupDebugMessenger();
			createSurface();
			pickPhysicalDevice();
			createLogicalDevice();
			createSwapChain();
			createImageViews();
			createGraphicsPipeline();
			createCommandPool();
			createCommandBuffers();
			createSyncObjects();
		}

		void createSyncObjects()
		{
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

		void createCommandBuffers()
		{
			vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = MAX_FRAMES_IN_FLIGHT };
			commandBuffers = vk::raii::CommandBuffers(device, allocInfo);
		}

		void createCommandPool()
		{
			vk::CommandPoolCreateInfo poolInfo
			{
				.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, 
				.queueFamilyIndex = queueIndex
			};

			commandPool = vk::raii::CommandPool(device, poolInfo);
		}

		void createGraphicsPipeline()
		{
			vk::raii::ShaderModule shaderModule = createShaderModule(readFile("Shaders/slang.spv"));

			vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule, .pName = "vertMain" };
			vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };
			vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
			vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList };
			vk::PipelineViewportStateCreateInfo viewPortState{ .viewportCount = 1, .scissorCount = 1 };

			vk::PipelineRasterizationStateCreateInfo rasterizer
			{
				.depthClampEnable = vk::False,
				.rasterizerDiscardEnable = vk::False,
				.polygonMode = vk::PolygonMode::eFill,
				.cullMode = vk::CullModeFlagBits::eBack,
				.frontFace = vk::FrontFace::eClockwise,
				.depthBiasEnable = vk::False,
				.lineWidth = 1.0f

			};

			vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };

			vk::PipelineColorBlendAttachmentState colorBlendAttachment
			{
				.blendEnable = vk::False,
				.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
			};

			vk::PipelineColorBlendStateCreateInfo colorBlending
			{
				.logicOpEnable = vk::False,
				.logicOp = vk::LogicOp::eCopy,
				.attachmentCount = 1,
				.pAttachments = &colorBlendAttachment
			};

			std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
			vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };

			vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 0, .pushConstantRangeCount = 0 };
			pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

			vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = 
			{
				{
					.stageCount = 2,
					.pStages = shaderStages,
					.pVertexInputState = &vertexInputInfo,
					.pInputAssemblyState = &inputAssembly,
					.pViewportState = &viewPortState,
					.pRasterizationState = &rasterizer,
					.pMultisampleState = &multisampling,
					.pColorBlendState = &colorBlending,
					.pDynamicState = &dynamicState,
					.layout = pipelineLayout,
					.renderPass = nullptr
				},
				{
					.colorAttachmentCount = 1,
					.pColorAttachmentFormats = &swapChainSurfaceFormat.format
				}
			};

			graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
		}

		[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const
		{
			vk::ShaderModuleCreateInfo createInfo{ .codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(code.data()) };
			vk::raii::ShaderModule shaderModule{ device, createInfo };

			return shaderModule;
		}

		void createImageViews()
		{
			assert(swapChainImageViews.empty());

			vk::ImageViewCreateInfo imageViewCreateInfo
			{
				.viewType = vk::ImageViewType::e2D,
				.format = swapChainSurfaceFormat.format,
				/*.components = 
				{ 
					vk::ComponentSwizzle::eIdentity, 
					vk::ComponentSwizzle::eIdentity, 
					vk::ComponentSwizzle::eIdentity,
					vk::ComponentSwizzle::eIdentity
				},*/
				.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
			};

			for (auto& image : swapChainImages)
			{
				imageViewCreateInfo.image = image;
				swapChainImageViews.emplace_back(device, imageViewCreateInfo);
			}
		}

		void cleanUpSwapChain()
		{
			swapChainImageViews.clear();
			swapChain = nullptr;
		}

		void recreateSwapChain()
		{
			int width = 0, height = 0;
			glfwGetFramebufferSize(window, &width, &height);
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(window, &width, &height);
				glfwWaitEvents();
			}

			device.waitIdle();

			cleanUpSwapChain();

			createSwapChain();
			createImageViews();
		}

		void createSwapChain()
		{
			vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
			swapChainExtent = chooseSwapChainExtent(surfaceCapabilities);
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

		static uint32_t chooseSwapChainMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
		{
			auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);

			if (surfaceCapabilities.maxImageCount > 0 && surfaceCapabilities.maxImageCount < minImageCount)
			{
				minImageCount = surfaceCapabilities.maxImageCount;
			}

			return minImageCount;
		}

		static vk::SurfaceFormatKHR chooseSwapChainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
		{
			assert(!availableFormats.empty());

			const auto formatIt = std::ranges::find_if(
				availableFormats,
				[](const auto& format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; }
			);

			return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
		}

		static vk::PresentModeKHR chooseSwapChainPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
		{
			assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));

			return std::ranges::any_of(availablePresentModes,
				[](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ?
				vk::PresentModeKHR::eMailbox :
				vk::PresentModeKHR::eFifo;
		}

		vk::Extent2D chooseSwapChainExtent(vk::SurfaceCapabilitiesKHR const& capabilities)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}

			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			return
			{
				std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
				std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
			};
		}

		void createSurface()
		{
			VkSurfaceKHR _surface;
			if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0)
			{
				throw std::runtime_error("Failed to create window surface!");
			}

			surface = vk::raii::SurfaceKHR(instance, _surface);
		}

		void createLogicalDevice()
		{
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
							   vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain =
			{
				{},
				{ .shaderDrawParameters = true },
				{ .synchronization2 = true, .dynamicRendering = true },
				{ .extendedDynamicState = true }
			};

			float queuePriority = 0.5f;
			vk::DeviceQueueCreateInfo deviceQueueCreateInfo { .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };
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

		void createInstance()
		{
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

			auto requiredExtensions = getRequiredInstanceExtensions();

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

		std::vector<const char*> getRequiredInstanceExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			
			std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
			if (enableValidationLayers)
			{
				extensions.push_back(vk::EXTDebugUtilsExtensionName);
			}

			return extensions;
		}

		void setupDebugMessenger()
		{
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

		static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
															  vk::DebugUtilsMessageTypeFlagsEXT type,
															  const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
															  void *pUserData)
		{
			if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError ||
				severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
			{
				std::cerr << "Validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
			}

			return vk::False;
		}

		void pickPhysicalDevice()
		{
			auto physicalDevices = instance.enumeratePhysicalDevices();
			auto const devIter = std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) { return isDeviceSuitable(physicalDevice); });

			if (devIter == physicalDevices.end())
			{
				throw std::runtime_error("Failed to find suitable GPU!");
			}

			physicalDevice = *devIter;
		}

		

		bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
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
			bool supportRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
										   features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
										   features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
										   features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

			return supportVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportRequiredFeatures;
		}

		void mainLoop()
		{
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
				drawFrame();
			}

			device.waitIdle();
		}

		void drawFrame()
		{
			auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
			if (fenceResult != vk::Result::eSuccess)
			{
				throw std::runtime_error("Failed to wait for Fence!");
			}

			auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

			if (result == vk::Result::eErrorOutOfDateKHR)
			{
				recreateSwapChain();
				return;
			}

			if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			{
				assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
				throw std::runtime_error("Failed to acquire swap cahin image!");
			}

			device.resetFences(*inFlightFences[frameIndex]);

			commandBuffers[frameIndex].reset();
			recordCommandBuffer(imageIndex);

			//graphicsQueue.waitIdle();

			vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
			const vk::SubmitInfo submitInfo
			{
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
				.pWaitDstStageMask = &waitDestinationStageMask,
				.commandBufferCount = 1,
				.pCommandBuffers = &*commandBuffers[frameIndex],
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
				recreateSwapChain();
			}
			else
			{
				assert(result == vk::Result::eSuccess);
			}

			frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
		}

		void recordCommandBuffer(uint32_t imageIndex)
		{
			commandBuffers[frameIndex].begin({});

			transitionImageLayout(
				imageIndex,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eColorAttachmentOptimal,
				{},
				vk::AccessFlagBits2::eColorAttachmentWrite,
				vk::PipelineStageFlagBits2::eColorAttachmentOutput,
				vk::PipelineStageFlagBits2::eColorAttachmentOutput
			);

			vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
			vk::RenderingAttachmentInfo attachmentInfo =
			{
				.imageView = swapChainImageViews[imageIndex],
				.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
				.loadOp = vk::AttachmentLoadOp::eClear,
				.storeOp = vk::AttachmentStoreOp::eStore,
				.clearValue = clearColor
			};

			vk::RenderingInfo renderingInfo =
			{
				.renderArea = { .offset = { 0, 0 }, .extent = swapChainExtent },
				.layerCount = 1,
				.colorAttachmentCount = 1,
				.pColorAttachments = &attachmentInfo
			};

			commandBuffers[frameIndex].beginRendering(renderingInfo);

			commandBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);

			commandBuffers[frameIndex].setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
			commandBuffers[frameIndex].setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));

			commandBuffers[frameIndex].draw(3, 1, 0, 0);

			commandBuffers[frameIndex].endRendering();

			transitionImageLayout(
				imageIndex,
				vk::ImageLayout::eColorAttachmentOptimal,
				vk::ImageLayout::ePresentSrcKHR,
				vk::AccessFlagBits2::eColorAttachmentWrite,
				{},
				vk::PipelineStageFlagBits2::eColorAttachmentOutput,
				vk::PipelineStageFlagBits2::eBottomOfPipe
			);

			commandBuffers[frameIndex].end();
		}

		void transitionImageLayout(uint32_t imageIndex, 
								   vk::ImageLayout oldLayout, 
								   vk::ImageLayout newLayout, 
								   vk::AccessFlags2 srcAccessMask, 
								   vk::AccessFlags2 dstAccessMask, 
								   vk::PipelineStageFlags2 srcStageMask, 
								   vk::PipelineStageFlags2 dstStageMask)
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
				.image = swapChainImages[imageIndex],
				.subresourceRange =
				{
					.aspectMask = vk::ImageAspectFlagBits::eColor,
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

			commandBuffers[frameIndex].pipelineBarrier2(dependencyInfo);
		}

		void cleanUp()
		{
			cleanUpSwapChain();

			glfwDestroyWindow(window);

			glfwTerminate();
		}
};

int main()
{

	try
	{
		HelloTirangleApp app;
		app.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
} 