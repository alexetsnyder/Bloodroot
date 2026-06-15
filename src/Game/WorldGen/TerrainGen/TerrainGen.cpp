#include "TerrainGen.h"

#include <map>

namespace Game
{
	std::map<VoxelType, Voxel> VoxelMap
	{
		{ VoxelType::AIR, { VoxelType::AIR, -1, -1, -1, -1, -1, -1 }},
		{ VoxelType::WATER, { VoxelType::WATER, 6, 6, 6, 6, 6, 6 }},
		{ VoxelType::GRASS, { VoxelType::GRASS, 1, 1, 2, 0, 1, 1 }},
		{ VoxelType::DIRT, { VoxelType::DIRT, 0, 0, 0, 0, 0, 0  }},
		{ VoxelType::STONE, { VoxelType::STONE, 3, 3, 3, 3, 3, 3 }},
		{ VoxelType::BEDROCK, { VoxelType::BEDROCK, 4, 4, 4, 4, 4, 4 }},
		{ VoxelType::SAND, { VoxelType::SAND, 5, 5, 5, 5, 5, 5 }}
	};

	TerrainGen::TerrainGen(uint32_t seed)
		: noise{ seed }
	{

	}

	TerrainGen::~TerrainGen()
	{

	}

	Voxel TerrainGen::GetVoxel(VoxelType voxelType)
	{
		return VoxelMap[voxelType];
	}

	Voxel TerrainGen::GetVoxel(int32_t yStart, const glm::vec3& position)
	{
		return GetVoxel(GetVoxelType(yStart, position));
	}

	VoxelType TerrainGen::GetVoxelType(int32_t yStart, const glm::vec3& position)
	{
		int32_t y = static_cast<int32_t>(std::floorf(position.y));

		uint32_t height = getHeight(position.x, position.z);

		return getVoxelType(yStart, y, height);
	}

	std::vector<VoxelType> TerrainGen::GetVoxelTypeColumn(int32_t yStart, int32_t yEnd, int32_t xPos, int32_t zPos)
	{
		auto voxelTypes = std::vector<VoxelType>{};

		auto height = getHeight(xPos, zPos);

		for (uint32_t y = yStart; y < yEnd; y++)
		{
			voxelTypes.push_back(getVoxelType(yStart, y, height));
		}

		return voxelTypes;
	}

	uint32_t TerrainGen::getHeight(int32_t xPos, int32_t zPos)
	{
		float noiseValue = noise.GetNoise(xPos, zPos);

		return static_cast<uint32_t>(std::floorf(noiseValue * VARY_HEIGHT + MIN_HEIGHT));
	}

	VoxelType TerrainGen::getVoxelType(int32_t yStart, int32_t yPos, uint32_t height) const
	{
		if (yPos < yStart)
		{
			return VoxelType::AIR;
		}

		if (yPos <= yStart)
		{
			return VoxelType::BEDROCK;
		}

		if (height >= DIRT_HEIGHT)
		{
			if (yPos > height)
			{
				return VoxelType::AIR;
			}
			if (yPos == height)
			{
				return VoxelType::GRASS;
			}
			else if (yPos >= DIRT_HEIGHT)
			{
				return VoxelType::DIRT;
			}
		}
		else if (height >= SAND_HEIGHT)
		{
			if (yPos > height)
			{
				return VoxelType::AIR;
			}
			else if (yPos >= SAND_HEIGHT)
			{
				return VoxelType::SAND;
			}
		}
		else //(height < WATER_HEIGHT)
		{
			if (yPos > WATER_HEIGHT)
			{
				return VoxelType::AIR;
			}
			else if (yPos > height)
			{
				return VoxelType::WATER;
			}
		}

		return VoxelType::STONE;
	}
}