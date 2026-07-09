#include "WorldGen.h"

#include "Common.h"
#include "Random.h"

#include <chrono>
#include <iostream>
#include <ranges>
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

	void WorldGen::GenerateTrees(const BuildInfo& buildInfo, std::unordered_map<glm::i32vec3, Chunk>& chunks) const
	{
		auto startPos = buildInfo.startPos;
		auto endPos = buildInfo.endPos;
		auto size = buildInfo.size;

		auto poissonStartInfo = Core::Math::PoissonStartInfo
		{
			.startPos = { startPos.x, startPos.z },
			.radius = 10.0f,
			.k = 30,
			.width = static_cast<float>(size.x),
			.height = static_cast<float>(size.z)
		};

		std::vector<glm::vec2> points{};
		Core::Math::Random::Instance().PoissonDiskSampling(poissonStartInfo, points);

		for (const auto& point : points)
		{
			auto colPos = glm::vec3{ point.x, 0.0f, point.y };

			if (IsInBounds(buildInfo, { point.x + 2, 0.0f, point.y }) &&
				IsInBounds(buildInfo, { point.x - 2, 0.0f, point.y }) &&
				IsInBounds(buildInfo, { point.x, 0.0f, point.y + 2 }) &&
				IsInBounds(buildInfo, { point.x, 0.0f, point.y - 2 }))
			{
				auto chunkId = Chunk::MapToChunkId(colPos);
				auto& chunk = chunks[chunkId];

				if (chunk.IsGround(colPos))
				{
					auto height = chunk.GroundHeight(colPos);

					if (height.has_value())
					{
						auto x = std::floor(point.x);
						auto z = std::floor(point.y);

						auto mainTrunk = std::vector<VoxelPos>
						{
							{ height.value() + 1, VoxelType::OAK_BARK },
							{ height.value() + 2, VoxelType::OAK_BARK },
							{ height.value() + 3, VoxelType::OAK_BARK },
							{ height.value() + 4, VoxelType::OAK_BARK },
							{ height.value() + 5, VoxelType::OAK_BARK },
							{ height.value() + 6, VoxelType::OAK_BARK },
							{ height.value() + 7, VoxelType::OAK_LEAVES },
						};

						chunk.SetVoxels(x, z, mainTrunk);

						auto treeSides = std::vector<VoxelPos>
						{
							{ height.value() + 5, VoxelType::OAK_LEAVES },
							{ height.value() + 6, VoxelType::OAK_LEAVES },
							{ height.value() + 7, VoxelType::OAK_LEAVES },
						};

						setVoxels(x + 1, z, treeSides, chunks);
						setVoxels(x - 1, z, treeSides, chunks);
						setVoxels(x, z + 1, treeSides, chunks);
						setVoxels(x, z - 1, treeSides, chunks);

						auto shortTreeSides = std::vector<VoxelPos>
						{
							{ height.value() + 5, VoxelType::OAK_LEAVES },
							{ height.value() + 6, VoxelType::OAK_LEAVES },
						};

						setVoxels(x + 1, z + 1, shortTreeSides, chunks);
						setVoxels(x + 1, z - 1, shortTreeSides, chunks);
						setVoxels(x - 1, z + 1, shortTreeSides, chunks);
						setVoxels(x - 1, z - 1, shortTreeSides, chunks);
						
						setVoxels(x + 2, z, shortTreeSides, chunks);
						setVoxels(x - 2, z, shortTreeSides, chunks);
						setVoxels(x, z + 2, shortTreeSides, chunks);
						setVoxels(x, z - 2, shortTreeSides, chunks);
					}
				}
			}
		}
	}

	void WorldGen::GenerateMeshes(const BuildInfo& buildInfo,
								  const std::unordered_map<glm::i32vec3, Chunk>& chunks,
								  std::unordered_map<glm::i32vec3, Core::VK::Mesh>& meshes,
								  std::unordered_map<glm::i32vec3, Core::VK::Mesh>& tMeshes)
	{
		auto startPos = buildInfo.startPos;
		auto endPos = buildInfo.endPos;
		auto size = buildInfo.size;

		std::vector<bool> visited(size.x * size.y * size.z, false);

		std::cout << "Created Visited map!\n";

		auto isInVisited = [&startPos, &endPos](const glm::i32vec3& cubePos)
			{
				return ((cubePos.x >= startPos.x && cubePos.x < endPos.x) &&
						(cubePos.y >= startPos.y && cubePos.y < endPos.y) &&
						(cubePos.z >= startPos.z && cubePos.z < endPos.z)
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

						if (adjVoxel.Type == VoxelType::AIR || adjVoxel.Type == VoxelType::WATER || adjVoxel.Type == VoxelType::OAK_LEAVES)
						{
							chunk.CreateFace(static_cast<CubeFace>(i), cubePos, currentVoxel, mesh);
						}
					}
				}
			}
		}
	}

	void WorldGen::GenerateMesh(const std::unordered_map<glm::i32vec3, Chunk>& chunks,
								const Chunk& chunk,
								Core::VK::Mesh& mesh,
								Core::VK::Mesh& tMesh)
	{
		auto chunkPos = chunk.Position();

		auto buildInfo = BuildInfo
		{
			.startPos = { chunkPos.x, chunkPos.y, chunkPos.z },
			.endPos = { chunkPos.x + CHUNK_WIDTH, chunkPos.y + CHUNK_HEIGHT, chunkPos.z + CHUNK_DEPTH },
			.size = { CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH },
		};

		auto startPos = buildInfo.startPos;
		auto endPos = buildInfo.endPos;
		auto size = buildInfo.size;

		std::vector<bool> visited(size.x * size.y * size.z, false);

		auto isInVisited = [&startPos, &endPos](const glm::i32vec3& cubePos)
			{
				return ((cubePos.x >= startPos.x && cubePos.x < endPos.x) &&
						(cubePos.y >= startPos.y && cubePos.y < endPos.y) &&
						(cubePos.z >= startPos.z && cubePos.z < endPos.z)
					);
			};

		std::stack<glm::i32vec3> cubes;
		cubes.push(startPos);
		visited[getIndex(buildInfo, startPos)] = true;

		glm::i32vec3 adjCubes[6];

		while (!cubes.empty())
		{
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
			if (chunk.IsInBounds(cubePos))
			{
				auto currentVoxel = getVoxel(buildInfo, chunks, cubePos);

				if (currentVoxel.Type == VoxelType::WATER)
				{
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
					for (int i = 0; i < 6; i++)
					{
						auto adjVoxel = getVoxel(buildInfo, chunks, adjCubes[i]);

						if (adjVoxel.Type == VoxelType::AIR || adjVoxel.Type == VoxelType::WATER || adjVoxel.Type == VoxelType::OAK_LEAVES)
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

	bool WorldGen::GetAdjChunks(const glm::vec3& position, std::vector<glm::i32vec3>& adjChunks)
	{
		bool success = false;

		auto currentChunkId = Chunk::MapToChunkId(position);

		auto x = static_cast<int32_t>(std::floorf(position.x));
		auto y = static_cast<int32_t>(std::floorf(position.y));
		auto z = static_cast<int32_t>(std::floorf(position.z));
		
		glm::i32vec3 adjCubes[6];

		getAdjCubes({ x, y, z }, adjCubes);

		for (const auto& adjCube : adjCubes)
		{
			auto adjChunkId = Chunk::MapToChunkId(adjCube);

			if (adjChunkId != currentChunkId && std::ranges::find_if(adjChunks, [&adjChunkId](const auto& cId) { return cId == adjChunkId; }) == adjChunks.end())
			{
				adjChunks.push_back(adjChunkId);
				success = true;
			}
		}

		return success;
	}

	size_t WorldGen::getIndex(const BuildInfo& buildInfo, const glm::i32vec3& cubePos) const
	{
		auto startPos = buildInfo.startPos;
		auto size = buildInfo.size;

		size_t xPrime = static_cast<size_t>(Core::Math::MapToPositiveRange(cubePos.x, startPos.x));
		size_t yPrime = static_cast<size_t>(Core::Math::MapToPositiveRange(cubePos.y, startPos.y));
		size_t zPrime = static_cast<size_t>(Core::Math::MapToPositiveRange(cubePos.z, startPos.z));

		return xPrime + size.x * (yPrime + size.y * zPrime);
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
		auto chunkId = Chunk::MapToChunkId(cubePos);

		if (!IsInBounds(buildInfo, cubePos) && !chunks.contains(chunkId))
		{
			return terrainGen.GetVoxel(0, cubePos);
		}
	
		auto voxelType = chunks.at(chunkId).GetVoxelType(cubePos);
		return terrainGen.GetVoxel(voxelType);
	}

	void WorldGen::setVoxels(int32_t xPos, int32_t zPos, const std::vector<VoxelPos>& voxelPos, std::unordered_map<glm::i32vec3, Chunk>& chunks) const
	{
		auto chunkId = Chunk::MapToChunkId({ xPos, 0, zPos });

		chunks[chunkId].SetVoxels(xPos, zPos, voxelPos);
	}
}