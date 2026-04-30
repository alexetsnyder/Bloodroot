target_sources(
	BloodrootApp
	PRIVATE 
		Camera/Camera.cpp
		WorldGen/Chunk.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
		Camera/Camera.h
		WorldGen/Chunk.h
)

target_include_directories(
	BloodrootApp
	PUBLIC
		Camera
		WorldGen
)