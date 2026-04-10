#include <cstdlib>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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

			uint32_t glfwExtensionCount = 0;
			auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			auto extensionProperties = context.enumerateInstanceExtensionProperties();
			for (uint32_t i = 0; i < glfwExtensionCount; ++i)
			{
				if (std::ranges::none_of(extensionProperties,
										 [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
										 { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
				{
					throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
				}
			}

			vk::InstanceCreateInfo createInfo
			{
				.pApplicationInfo = &appInfo,
				.enabledExtensionCount = glfwExtensionCount,
				.ppEnabledExtensionNames = glfwExtensions
			};

			instance = vk::raii::Instance(context, createInfo);
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