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

	TerrainGen::TerrainGen()
		: worldCenter{ { 0 } }, worldSize{ { 0 } }, biomeNoise{ 0 }, noise{ 42 }
	{

	}

	TerrainGen::TerrainGen(const glm::i32vec3 & worldCenter, const glm::u32vec3 & worldSize)
		: worldCenter{ worldCenter }, worldSize{ worldSize }, biomeNoise{ 0 }, noise{ 42 }
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
		int32_t y = static_cast<int32_t>(std::floorf(position.y));

		uint32_t biome = getBiome(position.x, position.z);
		uint32_t height = getHeight(position.x, position.z);

		return getVoxelType(y, biome, height);
	}

	std::vector<VoxelType> TerrainGen::GetVoxelTypeColumn(int32_t xPos, int32_t zPos)
	{
		auto voxelTypes = std::vector<VoxelType>{};

		auto biome = getBiome(xPos, zPos);
		auto height = getHeight(xPos, zPos);

		for (uint32_t y = worldCenter.y; y < worldCenter.y + worldSize.y; y++)
		{
			voxelTypes.push_back(getVoxelType(y, biome, height));
		}

		return voxelTypes;
	}

	uint32_t TerrainGen::getHeight(int32_t xPos, int32_t zPos)
	{
		float noiseValue = noise.GetNoise(xPos, zPos);

		return static_cast<uint32_t>(std::floorf(noiseValue * VARY_HEIGHT + MIN_HEIGHT));
	}

	uint32_t TerrainGen::getBiome(int32_t xPos, int32_t zPos)
	{
		float noiseValue = biomeNoise.GetNoise(xPos, zPos);

		return static_cast<uint32_t>(std::floorf(noiseValue * VARY_HEIGHT + MIN_HEIGHT));
	}

	VoxelType TerrainGen::getVoxelType(uint32_t yPos, uint32_t biome, uint32_t height) const
	{
		if (yPos <= worldCenter.y)
		{
			return VoxelType::BEDROCK;
		}

		if (height == WATER_HEIGHT)
		{
			if (yPos > WATER_HEIGHT)
			{
				return VoxelType::AIR;
			}
			else if (yPos >= WATER_HEIGHT - SAND_DEPTH)
			{
				return VoxelType::SAND;
			}
		}
		else if (height <= WATER_HEIGHT)
		{
			if (yPos > WATER_HEIGHT)
			{
				return VoxelType::AIR;
			}
			else if (yPos >= WATER_HEIGHT - WATER_DEPTH)
			{
				return VoxelType::WATER;
			}
			else if (yPos >= height - DIRT_DEPTH)
			{
				return VoxelType::DIRT;
			}
		}
		else
		{
			if (yPos > height)
			{
				return VoxelType::AIR;
			}
			if (yPos == height)
			{
				return VoxelType::GRASS;
			}
			else if (yPos >= height - DIRT_DEPTH)
			{
				return VoxelType::DIRT;
			}
		}

		return VoxelType::STONE;
	}
}