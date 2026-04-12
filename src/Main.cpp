#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_raii.hpp>

#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const std::vector<char const*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

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
		GLFWwindow* window = nullptr;

		vk::raii::Context context;
		vk::raii::Instance instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

		vk::raii::SurfaceKHR surface = nullptr;

		vk::raii::PhysicalDevice physicalDevice = nullptr;
		vk::raii::Device device = nullptr;

		vk::raii::Queue graphicsQueue = nullptr;

		vk::raii::SwapchainKHR swapChain = nullptr;
		std::vector<vk::Image> swapChainImages;
		vk::SurfaceFormatKHR swapChainSurfaceFormat;
		vk::Extent2D swapChainExtent;
		std::vector<vk::raii::ImageView> swapChainImageViews;

		std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

		void initWindow()
		{
			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Triangle!", nullptr, nullptr);
		}

		void initVulkan()
		{
			createInstance();
			setupDebugMessenger();
			createSurface();
			pickPhysicalDevice();
			createLogicalDevice();
			createSwapChain();
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

			uint32_t queueIndex = ~0;
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

			vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain =
			{
				{},
				{ .dynamicRendering = true },
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

			auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
			bool supportRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
				features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

			return supportVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportRequiredFeatures;
		}

		void mainLoop()
		{
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
			}
		}

		void cleanUp()
		{
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