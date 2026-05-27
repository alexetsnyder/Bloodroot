#include "WorldGen.h"

#include <chrono>
#include <iostream>
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

	void WorldGen::GenerateChunks(std::unordered_map<glm::i32vec3, Chunk>& chunks)
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

	void WorldGen::GenerateMeshes(const std::unordered_map<glm::i32vec3, Chunk>& chunks, std::unordered_map<glm::i32vec3, Core::Mesh>& meshes)
	{
		auto startPos = glm::i32vec3
		{
			worldCenter.x - worldSize.x / 2.0f - 1,
			worldCenter.y - 1,
			worldCenter.z - worldSize.z / 2.0f - 1
		};

		auto endPos = glm::i32vec3
		{
			worldCenter.x + worldSize.x / 2.0f,
			worldCenter.y + worldSize.y,
			worldCenter.z + worldSize.z / 2.0f,
		};

		std::vector<bool> visited((worldSize.x + 2) * (worldSize.y + 2) * (worldSize.z + 2), false);

		std::cout << "Created Visited map!\n";

		auto isInVisited = [&startPos, &endPos](const glm::i32vec3& cubePos)
			{
				return ((cubePos.x >= startPos.x && cubePos.x <= endPos.x) &&
					(cubePos.y >= startPos.y && cubePos.y <= endPos.y) &&
					(cubePos.z >= startPos.z && cubePos.z <= endPos.z)
					);
			};

		auto start = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();

		//Stack
		std::stack<glm::i32vec3> cubes;
		cubes.push(startPos);
		visited[getIndex(startPos)] = true;

		uint32_t voxelCount = 1;
		glm::i32vec3 adjCubes[6];

		auto tenPercent = static_cast<int32_t>(std::floorf(0.1f * visited.size()));

		while (!cubes.empty())
		{
			if (voxelCount++ % tenPercent == 0)
			{
				end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> elapsed = end - start;
				std::cout << "Completed Voxels " << voxelCount << " of " << visited.size();
				std::cout << " percent complete: " << (voxelCount / (float)visited.size()) * 100.0f;
				std::cout << " in " << (elapsed.count()) << "ms\n";

				start = std::chrono::high_resolution_clock::now();
			}

			auto cubePos = cubes.top();
			cubes.pop();

			getAdjCubes(cubePos, adjCubes);

			//Grab cube and add all neighbors to stack if not visited
			for (auto adjCubePos : adjCubes)
			{
				if (isInVisited(adjCubePos))
				{
					auto index = getIndex(adjCubePos);
					if (!visited[index])
					{
						visited[index] = true;
						cubes.push(adjCubePos);
					}
				}
			}

			//Check all cube sides
			if (IsInBounds(cubePos))
			{
				const auto chunkId = Chunk::MapToChunkId(cubePos);

				if (!meshes.contains(chunkId))
				{
					meshes.insert({ chunkId, Core::Mesh{} });
				}

				auto currentVoxel = getVoxel(chunks, cubePos);

				if (currentVoxel.Type != VoxelType::AIR)
				{
					const auto& chunk = chunks.at(chunkId);
					auto& mesh = meshes[chunkId];

					for (int i = 0; i < 6; i++)
					{
						auto adjVoxel = getVoxel(chunks, adjCubes[i]);

						if (adjVoxel.Type == VoxelType::AIR)
						{
							chunk.CreateFace(static_cast<CubeFace>(i), cubePos, currentVoxel, mesh);
						}
					}
				}
			}
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

	size_t WorldGen::getIndex(const glm::i32vec3& cubePos) const
	{
		size_t xPos = cubePos.x + worldSize.x / 2.0f + 1.0f;
		size_t yPos = cubePos.y + 1.0f;
		size_t zPos = cubePos.z + worldSize.z / 2.0f + 1.0f;

		return xPos + yPos * worldSize.x + zPos * worldSize.x * worldSize.y;
	}

	void WorldGen::generateChunk(const glm::i32vec3& position, std::unordered_map<glm::i32vec3, Chunk>& chunks)
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

	void WorldGen::getAdjCubes(const glm::i32vec3& cubePos, glm::i32vec3 adjCubes[])
	{
		//Left
		adjCubes[0] = cubePos + glm::i32vec3(-1, 0, 0);

		//Right
		adjCubes[1] = cubePos + glm::i32vec3(1, 0, 0);

		//Top
		adjCubes[2] = cubePos + glm::i32vec3(0, 1, 0);

		//Bottom
		adjCubes[3] = cubePos + glm::i32vec3(0, -1, 0);

		//Front
		adjCubes[4] = cubePos + glm::i32vec3(0, 0, 1);

		//Back
		adjCubes[5] = cubePos + glm::i32vec3(0, 0, -1);
	}

	Voxel WorldGen::getVoxel(const std::unordered_map<glm::i32vec3, Chunk>& chunks, const glm::i32vec3& cubePos)
	{
		if (!IsInBounds(cubePos))
		{
			return terrainGen.GetVoxel(cubePos);
		}

		auto chunkId = Chunk::MapToChunkId(cubePos);
		auto voxelType = chunks.at(chunkId).GetVoxelType(cubePos);
		return terrainGen.GetVoxel(voxelType);
	}
}