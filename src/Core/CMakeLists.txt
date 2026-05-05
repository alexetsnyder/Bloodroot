target_sources(
	BloodrootApp
	PRIVATE 
		Window/Window.cpp
		Window/GLFWInstance.cpp
		Vulkan/VulkanRenderer.cpp
		Image/Image.cpp
		"Math/Noise/SimplexNoise.cpp"
	PUBLIC
	FILE_SET HEADERS
	FILES
		Window/Window.h
		Interfaces/IRenderer.h
		Window/GLFWInstance.h 
		Vulkan/VulkanRenderer.h 
		Vulkan/Geometry/Square.h 
		Vulkan/Geometry/Vertex.h 
		Vulkan/Geometry/Cube.h 
		Vulkan/Geometry/Mesh.h 
		Image/Image.h
		"Math/Noise/FastNoiseLite.h"
		"Math/Noise/SimplexNoise.h"
)

target_include_directories(
	BloodrootApp
	PUBLIC
		Window
		Interfaces
		Vulkan
		Vulkan/Geometry
		Image
		"Math/Noise"
)

add_subdirectory(FileIO)
