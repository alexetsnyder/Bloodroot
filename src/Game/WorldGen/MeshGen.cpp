#include "MeshGen.h"

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

	MeshGen::MeshGen()
		: chunk{ 16, 16, 16, glm::vec3(0.0f, 0.0f, 0.0f) },
		  mesh{},
	      noise{ 42 }
		  
	{
		
	}

	struct I32Vec3Comparator
	{
		bool operator()(const glm::i32vec3& lhs, const glm::i32vec3& rhs) const
		{
			return (std::tie(lhs.x, lhs.y, lhs.z) < std::tie(rhs.x, rhs.y, rhs.z));
		}
	};

	void MeshGen::GenerateMesh()
	{
		int vertexCount = 0;

		//visited
		std::map<glm::i32vec3, bool, I32Vec3Comparator> visited;
		for (int x = -1; x <= chunk.Width(); x++)
		{
			for (int y = -1; y <= chunk.Height(); y++)
			{
				for (int z = -1; z <= chunk.Depth(); z++)
				{
					visited[glm::i32vec3(x, y, z)] = false;
				}
			}
		}

		//stack
		std::stack<glm::i32vec3> cubes;
		auto startCubePos = glm::i32vec3(-1, -1, -1);
		cubes.push(startCubePos);
		visited[startCubePos] = true;

		while (!cubes.empty())
		{
			auto cubePos = cubes.top();
			cubes.pop();

			auto adjCubes = getAdjCubes(cubePos);

			//grab cube and add all neighbors to stack if not visited
			for (auto adjCubePos : adjCubes)
			{
				if (isInBounds(adjCubePos) && !visited[adjCubePos])
				{
					visited[adjCubePos] = true;
					cubes.push(adjCubePos);
				}
			}

			//check all cube sides
			auto currentVoxel = getVoxel(cubePos);

			bool isCurrentCubeSolid = chunk.IsInBounds(cubePos) && currentVoxel.Type != VoxelType::AIR;
			if (isCurrentCubeSolid)
			{
				for (int i = 0; i < adjCubes.size(); i++)
				{
					CubeFace face = static_cast<CubeFace>(i);

					auto adjVoxel = getVoxel(adjCubes[i]);

					if (!chunk.IsInBounds(adjCubes[i]) || adjVoxel.Type == VoxelType::AIR)
					{
						chunk.CreateFace(face, cubePos, currentVoxel, mesh, vertexCount);
					}
				}
			}
		}
	}

	bool MeshGen::isInBounds(const glm::vec3& position)
	{
		if ((position.y >= -1 && position.y <= 16) &&
			(position.x >= -1 && position.x <= 16) &&
			(position.z >= -1 && position.z <= 16))
		{
			return true;
		}

		return false;
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

	Voxel MeshGen::getVoxel(const glm::vec3& position)
	{
		uint32_t y = static_cast<uint32_t>(std::floorf(position.y));

		if (y == 0)
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