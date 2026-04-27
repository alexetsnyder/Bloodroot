target_sources(
	BloodrootApp
	PRIVATE 
		Camera/Camera.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
		Camera/Camera.h
)

target_include_directories(
	BloodrootApp
	PUBLIC
		Camera
)