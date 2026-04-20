find_package(glfw3 CONFIG REQUIRED)

add_library(BloodrootApp)

target_link_libraries(BloodrootApp PRIVATE glfw)

target_sources(
	BloodrootApp
	PRIVATE 
		Window/Window.cpp
		Window/GLFWInstance.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
		Window/Window.h
		Interfaces/IRenderer.h
		Window/GLFWInstance.h 
)

target_include_directories(
	BloodrootApp
	PUBLIC
		Window
		Interfaces
)

add_subdirectory(FileIO)
