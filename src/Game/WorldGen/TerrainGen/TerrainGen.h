#pragma once

#include "SimplexNoise.h"
#include "Voxel.h"

#include <glm/glm.hpp>

#include <vector>

namespace Game
{
	class TerrainGen
	{
		public:
			static const uint32_t MIN_HEIGHT = 32;
			static const uint32_t VARY_HEIGHT = 16;
			static const uint32_t DIRT_DEPTH = 3;
			static const uint32_t WATER_HEIGHT = 32;
			static const uint32_t WATER_DEPTH = 1;
			static const uint32_t SAND_DEPTH = 1;

			TerrainGen();
			TerrainGen(const glm::i32vec3& worldCenter, const glm::u32vec3& worldSize);
			~TerrainGen();

			Voxel GetVoxel(VoxelType voxelType);
			Voxel GetVoxel(const glm::vec3& position);
			VoxelType GetVoxelType(const glm::vec3& position);
			std::vector<VoxelType> GetVoxelTypeColumn(int32_t xPos, int32_t zPos);

		private:
			glm::u32vec3 worldSize;
			glm::i32vec3 worldCenter;
			Core::Math::SimplexNoise biomeNoise;
			Core::Math::SimplexNoise noise;

			uint32_t getHeight(int32_t xPos, int32_t zPos);
			uint32_t getBiome(int32_t xPos, int32_t zPos);
			VoxelType getVoxelType(uint32_t yPos, uint32_t biome, uint32_t height) const;
	};
}