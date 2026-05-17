#include "MeshGen.h"

#include "GLMExtensions.h"

#include <map>
#include <stack>

namespace Game
{
	std::map<VoxelType, Voxel> voxels
	{
		{ VoxelType::AIR, { VoxelType::AIR, -1, -1, -1, -1, -1, -1 } },
		{ VoxelType::GRASS, { VoxelType::GRASS, 1, 1, 2, 0, 1, 1 } },
		{ VoxelType::DIRT, { VoxelType::DIRT, 0, 0, 0, 0, 0, 0  } },
		{ VoxelType::STONE, { VoxelType::STONE, 3, 3, 3, 3, 3, 3 } },
		{ VoxelType::BEDROCK, { VoxelType::BEDROCK, 4, 4, 4, 4, 4, 4 } },
	};

	MeshGen::MeshGen(const glm::i32vec3& worldCenter, const glm::u32vec3& worldSize, const glm::u32vec3& chunkSize)
		: noise{ 42 },
		  worldCenter{ worldCenter },
		  worldSize{ worldSize },
		  chunkSize{ chunkSize }
	{
		
	}

	void MeshGen::GenerateChunkMeshes(std::vector<ChunkMesh>& chunkMeshes)
	{
		uint32_t vertexCount = 0;

		int32_t xPos = worldCenter.x;
		int32_t yPos = worldCenter.y;
		int32_t zPos = worldCenter.z;

		int32_t xStartPos = xPos - static_cast<int32_t>(worldSize.x) / 2;
		int32_t xEndPos = xPos + static_cast<int32_t>(worldSize.x) / 2;

		int32_t yStartPos = yPos;
		int32_t yEndPos = yPos + static_cast<int32_t>(worldSize.y);

		int32_t zStartPos = zPos - static_cast<int32_t>(worldSize.z) / 2;
		int32_t zEndPos = zPos + static_cast<int32_t>(worldSize.z) / 2;

		//Visited
		std::map<glm::i32vec3, bool, Core::Ext::I32Vec3Comparator> visited;
		for (int32_t x = xStartPos; x < xEndPos; x += chunkSize.x)
		{
			for (int32_t y = yStartPos; y < yEndPos; y += chunkSize.y)
			{
				for (int32_t z = zStartPos; z < zEndPos; z += chunkSize.z)
				{
					visited[glm::i32vec3(x, y, z)] = false;
				}
			}
		}

		//Stack
		std::stack<glm::i32vec3> chunkStack;
		auto startPos = glm::i32vec3(xPos - worldSize.x / 2, yPos, zPos - worldSize.z / 2);
		chunkStack.push(startPos);
		visited[startPos] = true;

		while (!chunkStack.empty())
		{
			auto chunkPos = chunkStack.top();
			chunkStack.pop();

			auto chunkMesh = ChunkMesh
			{
				Chunk{ chunkSize.x, chunkSize.y, chunkSize.z, chunkPos },
				Core::Mesh{},
			};

			chunkMeshes.push_back(chunkMesh);

			generateChunkMesh(chunkMeshes, vertexCount);

			auto adjChunks = getAdjChunks(chunkMeshes.back());

			for (const auto& adjChunkPos : adjChunks)
			{
				if (isInBounds(adjChunkPos) && !visited[adjChunkPos])
				{
					visited[adjChunkPos] = true;
					chunkStack.push(adjChunkPos);
				}
			}
		}
	}

	bool MeshGen::isInBounds(const glm::vec3& position) const
	{
		if ((position.y >= worldCenter.y && position.y < worldCenter.y + worldSize.y) &&
			(position.x >= worldCenter.x - worldSize.x / 2.0f && position.x < worldCenter.x + worldSize.x / 2.0f) &&
			(position.z >= worldCenter.z - worldSize.z / 2.0f && position.z < worldCenter.z + worldSize.z / 2.0f))
		{
			return true;
		}

		return false;
	}

	std::vector<glm::i32vec3> MeshGen::getAdjChunks(const ChunkMesh& chunkMesh)
	{
		uint32_t width = chunkMesh.chunk.Width();
		uint32_t height = chunkMesh.chunk.Height();
		uint32_t depth = chunkMesh.chunk.Depth();

		std::vector<glm::i32vec3> adjChunks;

		auto firstRow = getAdjChunkRow(chunkMesh, -height, false);
		auto secondRow = getAdjChunkRow(chunkMesh, 0, true);
		auto thirdRow = getAdjChunkRow(chunkMesh, height, false);

		adjChunks.insert(adjChunks.end(), firstRow.begin(), firstRow.end());
		adjChunks.insert(adjChunks.end(), secondRow.begin(), secondRow.end());
		adjChunks.insert(adjChunks.end(), thirdRow.begin(), thirdRow.end());

		return adjChunks;
	}

	std::vector<glm::i32vec3> MeshGen::getAdjChunkRow(const ChunkMesh& chunkMesh, uint32_t height, bool excludeMiddle)
	{
		uint32_t width = chunkMesh.chunk.Width();
		uint32_t depth = chunkMesh.chunk.Depth();
		auto chunkPos = chunkMesh.chunk.ChunkId();

		std::vector<glm::i32vec3> adjChunks;

		//Left Back
		adjChunks.push_back(chunkPos + glm::i32vec3(-width, height, -depth));

		//Back
		adjChunks.push_back(chunkPos + glm::i32vec3(0, height, -depth));

		//Right Back
		adjChunks.push_back(chunkPos + glm::i32vec3(width, height, -depth));

		//Left
		adjChunks.push_back(chunkPos + glm::i32vec3(-width, height, 0));

		if (!excludeMiddle)
		{
			//Middle
			adjChunks.push_back(chunkPos + glm::i32vec3(0, height, 0));
		}
		
		//Right
		adjChunks.push_back(chunkPos + glm::i32vec3(width, height, 0));

		//Left Front
		adjChunks.push_back(chunkPos + glm::i32vec3(-width, height, depth));

		//Front
		adjChunks.push_back(chunkPos + glm::i32vec3(0, height, depth));

		//Right Front
		adjChunks.push_back(chunkPos + glm::i32vec3(width, height, depth));

		return adjChunks;
	}

	std::vector<glm::i32vec3> MeshGen::getAdjCubes(const glm::i32vec3& cubePos)
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

	void MeshGen::generateChunkMesh(std::vector<ChunkMesh>& chunkMeshes, uint32_t& vertexCount)
	{
		auto& chunkMesh = chunkMeshes.back();

		auto xPos = static_cast<int32_t>(std::floorf(chunkMesh.chunk.Position().x));
		auto yPos = static_cast<int32_t>(std::floorf(chunkMesh.chunk.Position().y));
		auto zPos = static_cast<int32_t>(std::floorf(chunkMesh.chunk.Position().z));
		auto chunkSize = glm::i32vec3{ chunkMesh.chunk.ChunkSize() };

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
			auto currentVoxel = getVoxel(cubePos);

			bool isCurrentCubeSolid = chunkMesh.chunk.IsInBounds(cubePos) && currentVoxel.Type != VoxelType::AIR;
			if (isCurrentCubeSolid)
			{
				for (int i = 0; i < adjCubes.size(); i++)
				{
					auto adjVoxel = getVoxel(adjCubes[i]);

					if (!isInBounds(adjCubes[i]) || adjVoxel.Type == VoxelType::AIR)
					{
						CubeFace face = static_cast<CubeFace>(i);

						chunkMesh.chunk.CreateFace(face, cubePos, currentVoxel, chunkMesh.mesh);
					}
				}
			}
		}
	}

	Voxel MeshGen::getVoxel(const glm::vec3& position)
	{
		uint32_t y = static_cast<uint32_t>(std::floorf(position.y));

		if (y < worldCenter.y)
		{
			return voxels[VoxelType::AIR];
		}

		if (y == worldCenter.y)
		{
			return voxels[VoxelType::BEDROCK];
		}

		float noiseValue = noise.GetNoise(position.x, position.z);

		int height = std::floor(noiseValue * VARY_HEIGHT + MIN_HEIGHT);

		if (y > height)
		{
			return voxels[VoxelType::AIR];
		}
		else if (y == height)
		{
			return voxels[VoxelType::GRASS];
		}
		else if (y >= height - DIRT_DEPTH)
		{
			return voxels[VoxelType::DIRT];
		}
		else
		{
			return voxels[VoxelType::STONE];
		}
	}
}