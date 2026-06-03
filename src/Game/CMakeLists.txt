target_sources(
	BloodrootApp
	PRIVATE 
		Camera/Camera.cpp
		WorldGen/TerrainGen/Chunk.cpp	
		WorldGen/WorldGen.cpp
		WorldGen/TerrainGen/TerrainGen.cpp
		
	PUBLIC
	FILE_SET HEADERS
	FILES
		Camera/Camera.h
		WorldGen/TerrainGen/Chunk.h
		WorldGen/TerrainGen/Voxel.h
		WorldGen/ChunkIndicies.h
		WorldGen/WorldGen.h
		WorldGen/TerrainGen/TerrainGen.h
		
)

target_include_directories(
	BloodrootApp
	PUBLIC
		Camera
		GUI
		WorldGen
		WorldGen/TerrainGen
)