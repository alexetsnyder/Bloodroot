#include "Chunk.h"

#include <map>

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