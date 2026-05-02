target_sources(
	BloodrootApp
	PRIVATE 
		Camera/Camera.cpp
		WorldGen/Chunk.cpp
		WorldGen/MeshGen.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
		Camera/Camera.h
		WorldGen/Chunk.h
		WorldGen/Voxel.h
		WorldGen/MeshGen.h
)

target_include_directories(
	BloodrootApp
	PUBLIC
		Camera
		WorldGen
)