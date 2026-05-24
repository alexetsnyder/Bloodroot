#pragma once

#include "SimplexNoise.h"
#include "Voxel.h"

#include <glm/glm.hpp>

namespace Game
{
	class TerrainGen
	{
		public:
			static const uint32_t MIN_HEIGHT = 32;
			static const uint32_t VARY_HEIGHT = 16;
			static const uint32_t DIRT_DEPTH = 3;

			TerrainGen();
			TerrainGen(const glm::i32vec3& worldCenter, const glm::u32vec3& worldSize);
			~TerrainGen();

			Voxel GetVoxel(VoxelType voxelType);
			Voxel GetVoxel(const glm::vec3& position);
			VoxelType GetVoxelType(const glm::vec3& position);

		private:
			glm::u32vec3 worldSize;
			glm::i32vec3 worldCenter;
			Core::Math::SimplexNoise noise;
	};
}