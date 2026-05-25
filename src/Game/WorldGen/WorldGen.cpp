#include "WorldGen.h"

#include <stack>

namespace Game
{
	WorldGen::WorldGen()
		: worldCenter{{ 0.0f }}, worldSize{{ 0 }}, terrainGen{}
	{
		
	}

	WorldGen::WorldGen(const glm::vec3& center, const glm::vec3& size)
		: worldCenter{ center }, worldSize{ size }, terrainGen{ center, size }
	{
		
	}

	WorldGen::~WorldGen()
	{

	}

	void WorldGen::GenerateChunks(std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks)
	{
		auto startPos = glm::i32vec3
		{
			static_cast<int32_t>(worldCenter.x) - worldSize.x / 2,
			static_cast<int32_t>(worldCenter.y),
			static_cast<int32_t>(worldCenter.z) - worldSize.z / 2,
		};

		auto endPos = glm::i32vec3
		{
			static_cast<int32_t>(worldCenter.x) + worldSize.x / 2,
			static_cast<int32_t>(worldCenter.y) + worldSize.y,
			static_cast<int32_t>(worldCenter.z) + worldSize.z / 2,
		};

		for (int32_t y = startPos.y; y < endPos.y; y += CHUNK_HEIGHT)
		{
			for (int32_t x = startPos.x; x < endPos.x; x += CHUNK_WIDTH)
			{
				for (int32_t z = startPos.z; z < endPos.z; z += CHUNK_DEPTH)
				{
					generateChunk(glm::i32vec3{ x, y, z }, chunks);
				}
			}
		}
	}

	void WorldGen::GenerateMeshes(const std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks, 
								  std::vector<ChunkMesh>& chunkMeshes)
	{
		for (const auto& [chunkId, chunk] : chunks)
		{
			generateMesh(chunks, chunk, chunkMeshes);
		}
	}

	bool WorldGen::IsInBounds(const glm::vec3& position) const
	{
		if ((position.y >= worldCenter.y && position.y < worldCenter.y + worldSize.y) &&
			(position.x >= worldCenter.x - worldSize.x / 2.0f && position.x < worldCenter.x + worldSize.x / 2.0f) &&
			(position.z >= worldCenter.z - worldSize.z / 2.0f && position.z < worldCenter.z + worldSize.z / 2.0f))
		{
			return true;
		}

		return false;
	}

	void WorldGen::generateChunk(const glm::i32vec3& position, std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks)
	{
		auto chunk = Chunk{ position };

		for (int32_t x = position.x; x < position.x + static_cast<int32_t>(CHUNK_WIDTH); x++)
		{
			for (int32_t z = position.z; z < position.z + static_cast<int32_t>(CHUNK_DEPTH); z++)
			{
				auto voxelTypes = terrainGen.GetVoxelTypeColumn(x, z);
				chunk.AddVoxelColumn(x, position.y, z, voxelTypes);
			}
		}

		chunks[chunk.ChunkId()] = chunk;
	}

	std::vector<glm::i32vec3> WorldGen::getAdjCubes(const glm::i32vec3& cubePos)
	{
		std::vector<glm::i32vec3> adjCubes;

		//Left
		adjCubes.push_back(cubePos + glm::i32vec3(-1, 0, 0));

		//Right
		adjCubes.push_back(cubePos + glm::i32vec3(1, 0, 0));

		//Top
		adjCubes.push_back(cubePos + glm::i32vec3(0, 1, 0));

		//Bottom
		adjCubes.push_back(cubePos + glm::i32vec3(0, -1, 0));

		//Front
		adjCubes.push_back(cubePos + glm::i32vec3(0, 0, 1));

		//Back
		adjCubes.push_back(cubePos + glm::i32vec3(0, 0, -1));

		return adjCubes;
	}

	Voxel WorldGen::getVoxel(const std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks,
							 const glm::i32vec3& cubePos)
	{
		if (!IsInBounds(cubePos))
		{
			return terrainGen.GetVoxel(cubePos);
		}

		auto chunkId = Chunk::MapToChunkId(cubePos);
		auto voxelType = chunks.at(chunkId).GetVoxelType(cubePos);
		return terrainGen.GetVoxel(voxelType);
	}

	void WorldGen::generateMesh(const std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks, 
								const Chunk& chunk, std::vector<ChunkMesh>& chunkMeshes)
	{
		auto chunkMesh = ChunkMesh
		{
			chunk.ChunkId(),
			Core::Mesh{},
		};

		auto xPos = static_cast<int32_t>(std::floorf(chunk.Position().x));
		auto yPos = static_cast<int32_t>(std::floorf(chunk.Position().y));
		auto zPos = static_cast<int32_t>(std::floorf(chunk.Position().z));
		auto chunkSize = glm::i32vec3{ CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH };

		//Visited
		std::map<glm::i32vec3, bool, Core::Ext::I32Vec3Comparator> visited;
		for (int32_t x = xPos - 1; x <= xPos + chunkSize.x; x++)
		{
			for (int32_t y = yPos - 1; y <= yPos + chunkSize.y; y++)
			{
				for (int32_t z = zPos - 1; z <= zPos + chunkSize.z; z++)
				{
					visited[glm::i32vec3(x, y, z)] = false;
				}
			}
		}

		auto isInVisited = [xPos, yPos, zPos, chunkSize](const glm::i32vec3& cubePos)
			{
				return ((cubePos.x >= xPos - 1 && cubePos.x <= xPos + chunkSize.x) &&
						(cubePos.y >= yPos - 1 && cubePos.y <= yPos + chunkSize.y) &&
						(cubePos.z >= zPos - 1 && cubePos.z <= zPos + chunkSize.z)
				);
			};

		//Stack
		std::stack<glm::i32vec3> cubes;
		auto startPos = glm::i32vec3(xPos, yPos, zPos);
		cubes.push(startPos);
		visited[startPos] = true;

		while (!cubes.empty())
		{
			auto cubePos = cubes.top();
			cubes.pop();

			auto adjCubes = getAdjCubes(cubePos);

			//Grab cube and add all neighbors to stack if not visited
			for (auto adjCubePos : adjCubes)
			{
				if (isInVisited(adjCubePos) && !visited[adjCubePos])
				{
					visited[adjCubePos] = true;
					cubes.push(adjCubePos);
				}
			}

			//Check all cube sides
			auto currentVoxel = getVoxel(chunks, cubePos);
			bool isCurrentCubeSolid = chunk.IsInBounds(cubePos) && currentVoxel.Type != VoxelType::AIR;

			if (isCurrentCubeSolid)
			{
				for (int i = 0; i < adjCubes.size(); i++)
				{
					auto adjVoxel = getVoxel(chunks, adjCubes[i]);

					if (!IsInBounds(adjCubes[i]) || adjVoxel.Type == VoxelType::AIR)
					{
						CubeFace face = static_cast<CubeFace>(i);

						chunk.CreateFace(face, cubePos, currentVoxel, chunkMesh.mesh);
					}
				}
			}
		}

		chunkMeshes.push_back(chunkMesh);
	}
}