#include "WorldGen.h"

#include <chrono>
#include <iostream>
#include <stack>

namespace Game
{
	WorldGen::WorldGen()
		: worldCenter{{ 0.0f }}, terrainGen{}
	{
		
	}

	WorldGen::WorldGen(const glm::vec3& center)
		: worldCenter{ center }, terrainGen{}
	{
		
	}

	WorldGen::~WorldGen()
	{

	}

	void WorldGen::GenerateChunks(const BuildInfo& buildInfo, std::unordered_map<glm::i32vec3, Chunk>& chunks)
	{
		auto startPos = buildInfo.startPos;
		auto endPos = buildInfo.endPos;

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

	void WorldGen::GenerateMeshes(const BuildInfo& buildInfo,
								  const std::unordered_map<glm::i32vec3, Chunk>& chunks,
								  std::unordered_map<glm::i32vec3, Core::VK::Mesh>& meshes,
								  std::unordered_map<glm::i32vec3, Core::VK::Mesh>& tMeshes)
	{
		auto startPos = buildInfo.startPos - 1;
		auto endPos = buildInfo.endPos;
		auto size = buildInfo.size;

		std::vector<bool> visited((size.x + 2) * (size.y + 2) * (size.z + 2), false);

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
		visited[getIndex(buildInfo, startPos)] = true;

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
					auto index = getIndex(buildInfo, adjCubePos);
					if (!visited[index])
					{
						visited[index] = true;
						cubes.push(adjCubePos);
					}
				}
			}

			//Check all cube sides
			if (IsInBounds(buildInfo, cubePos))
			{
				const auto chunkId = Chunk::MapToChunkId(cubePos);
				auto currentVoxel = getVoxel(buildInfo, chunks, cubePos);

				if (currentVoxel.Type == VoxelType::WATER)
				{
					if (!tMeshes.contains(chunkId))
					{
						tMeshes.insert({ chunkId, Core::VK::Mesh{} });
					}

					const auto& chunk = chunks.at(chunkId);
					auto& tMesh = tMeshes[chunkId];

					for (int i = 0; i < 6; i++)
					{
						auto adjVoxel = getVoxel(buildInfo, chunks, adjCubes[i]);

						if (adjVoxel.Type == VoxelType::AIR)
						{
							chunk.CreateFace(static_cast<CubeFace>(i), cubePos, currentVoxel, tMesh);
						}
					}
				}
				else if (currentVoxel.Type != VoxelType::AIR)
				{
					if (!meshes.contains(chunkId))
					{
						meshes.insert({ chunkId, Core::VK::Mesh{} });
					}

					const auto& chunk = chunks.at(chunkId);
					auto& mesh = meshes[chunkId];

					for (int i = 0; i < 6; i++)
					{
						auto adjVoxel = getVoxel(buildInfo, chunks, adjCubes[i]);

						if (adjVoxel.Type == VoxelType::AIR || adjVoxel.Type == VoxelType::WATER)
						{
							chunk.CreateFace(static_cast<CubeFace>(i), cubePos, currentVoxel, mesh);
						}
					}
				}
			}
		}
	}

	bool WorldGen::IsInBounds(const BuildInfo& buildInfo, const glm::vec3& position) const
	{

		auto startPos = buildInfo.startPos;
		auto endPos = buildInfo.endPos;

		if ((position.y >= startPos.y && position.y < endPos.y) &&
			(position.x >= startPos.x && position.x < endPos.x) &&
			(position.z >= startPos.z && position.z < endPos.z))
		{
			return true;
		}

		return false;
	}

	size_t WorldGen::getIndex(const BuildInfo& buildInfo, const glm::i32vec3& cubePos) const
	{
		auto startPos = buildInfo.startPos;

		size_t xPos = cubePos.x - startPos.x + 1.0f;
		size_t yPos = cubePos.y - startPos.y + 1.0f;
		size_t zPos = cubePos.z - startPos.z + 1.0f;

		auto size = buildInfo.size;

		return xPos + yPos * size.x + zPos * size.x * size.y;
	}

	void WorldGen::generateChunk(const glm::i32vec3& position, std::unordered_map<glm::i32vec3, Chunk>& chunks)
	{
		auto chunk = Chunk{ position };

		for (int32_t x = position.x; x < position.x + static_cast<int32_t>(CHUNK_WIDTH); x++)
		{
			for (int32_t z = position.z; z < position.z + static_cast<int32_t>(CHUNK_DEPTH); z++)
			{
				auto voxelTypes = terrainGen.GetVoxelTypeColumn(0, CHUNK_HEIGHT, x, z);
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

	Voxel WorldGen::getVoxel(const BuildInfo& buildInfo, const std::unordered_map<glm::i32vec3, Chunk>& chunks, const glm::i32vec3& cubePos)
	{
		if (!IsInBounds(buildInfo, cubePos))
		{
			return terrainGen.GetVoxel(0, cubePos);
		}

		auto chunkId = Chunk::MapToChunkId(cubePos);
		auto voxelType = chunks.at(chunkId).GetVoxelType(cubePos);
		return terrainGen.GetVoxel(voxelType);
	}
}