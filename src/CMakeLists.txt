find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Stb REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

add_library(BloodrootApp)

target_link_libraries(BloodrootApp PUBLIC glfw)
target_link_libraries(BloodrootApp PRIVATE glm::glm)
target_link_libraries(BloodrootApp PUBLIC FileIO)
target_link_libraries(BloodrootApp PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

target_sources(
	BloodrootApp
	PRIVATE 
		BloodrootApp.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
		BloodrootApp.h
		AppData.h
)

target_include_directories(BloodrootApp PRIVATE ${Stb_INCLUDE_DIR})

add_subdirectory(Core)
add_subdirectory(Game)