#include "TerrainGen.h"

#include <map>

namespace Game
{
	std::map<VoxelType, Voxel> VoxelMap
	{
		{ VoxelType::AIR, { VoxelType::AIR, -1, -1, -1, -1, -1, -1 } },
		{ VoxelType::GRASS, { VoxelType::GRASS, 1, 1, 2, 0, 1, 1 } },
		{ VoxelType::DIRT, { VoxelType::DIRT, 0, 0, 0, 0, 0, 0  } },
		{ VoxelType::STONE, { VoxelType::STONE, 3, 3, 3, 3, 3, 3 } },
		{ VoxelType::BEDROCK, { VoxelType::BEDROCK, 4, 4, 4, 4, 4, 4 } },
	};

	TerrainGen::TerrainGen()
		: worldCenter{ { 0 } }, worldSize{ { 0 } }, noise{ 42 }
	{

	}

	TerrainGen::TerrainGen(const glm::i32vec3 & worldCenter, const glm::u32vec3 & worldSize)
		: worldCenter{ worldCenter }, worldSize{ worldSize }, noise{ 42 }
	{

	}

	TerrainGen::~TerrainGen()
	{

	}

	Voxel TerrainGen::GetVoxel(VoxelType voxelType)
	{
		return VoxelMap[voxelType];
	}

	Voxel TerrainGen::GetVoxel(const glm::vec3& position)
	{
		return GetVoxel(GetVoxelType(position));
	}

	VoxelType TerrainGen::GetVoxelType(const glm::vec3& position)
	{
		uint32_t y = static_cast<uint32_t>(std::floorf(position.y));

		if (y < worldCenter.y)
		{
			return VoxelType::AIR;
		}

		if (y == worldCenter.y)
		{
			return VoxelType::BEDROCK;
		}

		float noiseValue = noise.GetNoise(position.x, position.z);

		int height = std::floor(noiseValue * VARY_HEIGHT + MIN_HEIGHT);

		if (y > height)
		{
			return VoxelType::AIR;
		}
		else if (y == height)
		{
			return VoxelType::GRASS;
		}
		else if (y >= height - DIRT_DEPTH)
		{
			return VoxelType::DIRT;
		}
		else
		{
			return VoxelType::STONE;
		}
	}

}