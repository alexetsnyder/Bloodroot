find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(Stb REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

add_library(BloodrootApp)

target_link_libraries(BloodrootApp PRIVATE glm::glm)
target_link_libraries(BloodrootApp PUBLIC FileIO)
target_link_libraries(BloodrootApp PUBLIC Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)
target_link_libraries(BloodrootApp PRIVATE SDL3::SDL3)

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