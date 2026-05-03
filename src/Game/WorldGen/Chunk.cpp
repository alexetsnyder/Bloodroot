#include "Chunk.h"

#include <map>
#include <stack>
#include <vector>

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

	Chunk::Chunk(int width, int height, int depth, glm::vec3 position)
	{
		this->width = width;
		this->height = height;
		this->depth = depth;
		this->position = position;
	}

	Chunk::~Chunk()
	{

	}

	bool Chunk::IsSolid(const glm::vec3& position)
	{
		glm::vec3 localPos = mapToLocal(position);

		if (localPos.y < 0)
		{
			return true;
		}

		if (localPos.x >= 16 || localPos.y >= 16 || localPos.z >= 16 ||
			localPos.x < 0 || localPos.z < 0)
		{
			return false;
		}

		return true;
	}

	void Chunk::generateMesh()
	{
		int vertexCount = 0;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int z = 0; z < depth; z++)
				{
					generateVoxel(position + glm::vec3(x, y, z), vertexCount);
				}
			}
		}
	}

	struct I32Vec3Comparator
	{
		bool operator()(const glm::i32vec3& lhs, const glm::i32vec3& rhs) const
		{
			return (std::tie(lhs.x, lhs.y, lhs.z) < std::tie(rhs.x, rhs.y, rhs.z));
		}
	};

	void Chunk::generateMesh2()
	{
		int vertexCount = 0;

		//visited
		std::map<glm::i32vec3, bool, I32Vec3Comparator> visited;
		for (int x = -1; x <= width; x++)
		{
			for (int y = -1; y <= height; y++)
			{
				for (int z = -1; z <= depth; z++)
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
				if (IsInBounds(adjCubePos) && !visited[adjCubePos])
				{
					visited[adjCubePos] = true;
					cubes.push(adjCubePos);
				}
			}

			//check all cube sides
			bool isCurrentCubeSolid = IsSolid(cubePos);
			if (isCurrentCubeSolid)
			{
				for (int i = 0; i < adjCubes.size(); i++)
				{
					CubeFace face = static_cast<CubeFace>(i);

					if (isCurrentCubeSolid != IsSolid(adjCubes[i]))
					{
						createFace(face, cubePos, vertexCount);
					}
				}
			}
		}
	}

	bool Chunk::IsSolid(const glm::i32vec3& cubePos)
	{
		/*if (cubePos.y < 0)
		{
			return true;
		}*/

		if (cubePos.x >= 16 || cubePos.y >= 16 || cubePos.z >= 16 ||
			cubePos.x < 0 || cubePos.z < 0 || cubePos.y < 0)
		{
			return false;
		}

		return true;
	}

	bool Chunk::IsInBounds(const glm::i32vec3& cubePos)
	{
		if ((cubePos.y >= -1 && cubePos.y <= 16) &&
			(cubePos.x >= -1 && cubePos.x <= 16) &&
			(cubePos.z >= -1 && cubePos.z <= 16))
		{
			return true;
		}

		return false;
	}

	std::vector<glm::i32vec3> Chunk::getAdjCubes(const glm::i32vec3& cubePos)
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

	void Chunk::createFace(CubeFace face, const glm::i32vec3& cubePos, int& vertexCount)
	{
		uint32_t x = cubePos.x;
		uint32_t y = cubePos.y;
		uint32_t z = cubePos.z;

		auto voxel = getVoxel(y);

		switch (face)
		{
			case CubeFace::LEFT:
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.leftFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.leftFaceIndex } });
				mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.leftFaceIndex } });
				mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.leftFaceIndex } });
				break;
			case CubeFace::RIGHT:
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.rightFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.rightFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 1.0f, 1.0f, voxel.rightFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.rightFaceIndex } });
				break;
			case CubeFace::TOP:
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 1.0f, 1.0f, voxel.topFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 0.0f, 1.0f, voxel.topFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.topFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.topFaceIndex } });
				break;
			case CubeFace::BOTTOM:
				mesh.AddVertex({ { x, y, z }, { 0.0f, 1.0f, voxel.bottomFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 1.0f, 1.0f, voxel.bottomFaceIndex } });
				mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.bottomFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.bottomFaceIndex } });
				break;
			case CubeFace::FRONT:
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 1.0f, 1.0f, voxel.frontFaceIndex } });
				break;
			case CubeFace::BACK:
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 0.0f, 0.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 0.0f, 1.0f, voxel.backFaceIndex } });
				break;
		}

		mesh.AddIndex(vertexCount);
		mesh.AddIndex(vertexCount + 1);
		mesh.AddIndex(vertexCount + 2);

		mesh.AddIndex(vertexCount + 2);
		mesh.AddIndex(vertexCount + 1);
		mesh.AddIndex(vertexCount + 3);

		vertexCount += 4;
	}

	glm::vec3 Chunk::mapToLocal(const glm::vec3& position)
	{
		float x = std::floorf(position.x - this->position.x);
		float y = std::floorf(position.y - this->position.y);
		float z = std::floorf(position.z - this->position.z);

		return glm::vec3(x, y, z);
	}

	void Chunk::generateVoxel(const glm::vec3& voxelPos, int& vertexCount)
	{
		uint32_t x = static_cast<uint32_t>(voxelPos.x);
		uint32_t y = static_cast<uint32_t>(voxelPos.y);
		uint32_t z = static_cast<uint32_t>(voxelPos.z);

		auto voxel = getVoxel(y);

		//Front Face
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.frontFaceIndex } });
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.frontFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.frontFaceIndex } });
		mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 1.0f, 1.0f, voxel.frontFaceIndex } });
		
		//Back Face
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.backFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 0.0f, 0.0f, voxel.backFaceIndex } });
		mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.backFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 0.0f, 1.0f, voxel.backFaceIndex } });

		//Top Face
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 1.0f, 1.0f, voxel.topFaceIndex } });
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 0.0f, 1.0f, voxel.topFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.topFaceIndex } });
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.topFaceIndex } });

		//Bottom Face
		mesh.AddVertex({ { x, y, z }, { 0.0f, 1.0f, voxel.bottomFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 1.0f, 1.0f, voxel.bottomFaceIndex } });
		mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.bottomFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.bottomFaceIndex } });

		//Left Face
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.leftFaceIndex } });
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.leftFaceIndex } });
		mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.leftFaceIndex } });
		mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.leftFaceIndex } });

		//Right Face
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.rightFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.rightFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 1.0f, 1.0f, voxel.rightFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.rightFaceIndex } });
		
		for (int i = 0; i < 6; i++)
		{
			mesh.AddIndex(vertexCount);
			mesh.AddIndex(vertexCount + 1);
			mesh.AddIndex(vertexCount + 2);

			mesh.AddIndex(vertexCount + 2);
			mesh.AddIndex(vertexCount + 1);
			mesh.AddIndex(vertexCount + 3);

			vertexCount += 4;
		}
	}

	Voxel Chunk::getVoxel(int y)
	{
		if (y >= 0 && y < 1)
		{
			return voxels[VoxelType::BEDROCK];
		}
		else if (y >= 1 && y < 10)
		{
			return voxels[VoxelType::STONE];
		}
		else if (y >= 10 && y < 15)
		{
			return voxels[VoxelType::DIRT];
		}
		else if (y >= 15 && y < 16)
		{
			return voxels[VoxelType::GRASS];
		}
		else
		{
			return voxels[VoxelType::AIR];
		}
		 
	}
}