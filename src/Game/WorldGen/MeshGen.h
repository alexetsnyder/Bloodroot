#pragma once

#include "Chunk.h"
#include "SimplexNoise.h"
#include "Voxel.h"

#include <glm/glm.hpp>
#include <vector>

namespace Game
{
	class MeshGen
	{
		public:
			const uint32_t MIN_HEIGHT = 6;
			const uint32_t VARY_HEIGHT = 6;
			const uint32_t DIRT_DEPTH = 3;

			MeshGen();

			const Core::Mesh& GetMesh() { return mesh; }

			void GenerateMesh();

		private:
			Core::Mesh mesh;
			Chunk chunk;
			Core::Math::SimplexNoise noise;

			bool isInBounds(const glm::vec3& position);
			std::vector<glm::i32vec3> getAdjCubes(const glm::i32vec3& cubePos);
			Voxel getVoxel(const glm::vec3& position);
	};
}
