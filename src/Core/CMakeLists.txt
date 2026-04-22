find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm REQUIRED)
find_package(stb REQUIRED)

add_library(BloodrootApp)

target_link_libraries(BloodrootApp PRIVATE glfw)
target_link_libraries(BloodrootApp PRIVATE glm::glm)
target_link_libraries(BloodrootApp PRIVATE FileIO)
target_link_libraries(Bloodroot PRIVATE Vulkan::Vulkan)

add_compile_definitions(VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

target_include_directories(BloodrootApp PRIVATE ${Stb_INCLUDE_DIR})

target_sources(
	BloodrootApp
	PRIVATE 
		Window/Window.cpp
		Window/GLFWInstance.cpp
		Vulkan/VulkanRenderer.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
		Window/Window.h
		Interfaces/IRenderer.h
		Window/GLFWInstance.h 
		Vulkan/VulkanRenderer.h 
		Vulkan/Geometry/Square.h 
		Vulkan/Geometry/Vertex.h)

target_include_directories(
	BloodrootApp
	PUBLIC
		Window
		Interfaces
		Vulkan
		Vulkan/Geometry
)

target_include_directories(BloodrootApp PRIVATE ${Stb_INCLUDE_DIR})

add_subdirectory(FileIO)
