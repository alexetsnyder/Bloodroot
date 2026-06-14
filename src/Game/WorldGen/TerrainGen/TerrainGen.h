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

			static const uint32_t WATER_HEIGHT = 32;
			static const uint32_t SAND_HEIGHT = 32;
			static const uint32_t DIRT_HEIGHT = 34;

			TerrainGen(uint32_t seed = 42);
			~TerrainGen();

			Voxel GetVoxel(VoxelType voxelType);
			Voxel GetVoxel(int32_t yStart, const glm::vec3& position);
			VoxelType GetVoxelType(int32_t yStart, const glm::vec3& position);
			std::vector<VoxelType> GetVoxelTypeColumn(int32_t yStart, int32_t yEnd, int32_t xPos, int32_t zPos);

		private:
			Core::Math::SimplexNoise noise;

			uint32_t getHeight(int32_t xPos, int32_t zPos);
			VoxelType getVoxelType(int32_t yStart, int32_t yPos, uint32_t height) const;
	};
}