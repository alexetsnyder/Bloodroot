find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image REQUIRED)

add_library(BloodrootApp)

target_link_libraries(BloodrootApp PRIVATE glm::glm)
target_link_libraries(BloodrootApp PUBLIC FileIO)
target_link_libraries(BloodrootApp PUBLIC Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)
target_link_libraries(BloodrootApp PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

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

add_subdirectory(Core)
add_subdirectory(Game)