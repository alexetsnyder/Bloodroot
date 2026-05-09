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
			const uint32_t MIN_HEIGHT = 32;
			const uint32_t VARY_HEIGHT = 16;
			const uint32_t DIRT_DEPTH = 3;

			MeshGen(const glm::i32vec3& worldCenter, const glm::u32vec3& worldSize, const glm::u32vec3& chunkSize);

			const Core::Mesh& GetMesh() { return mesh; }

			void GenerateMesh();

		private:
			glm::u32vec3 worldSize;
			glm::i32vec3 worldCenter;
			glm::u32vec3 chunkSize;
			Core::Mesh mesh;
			std::vector<Chunk> chunks;
			Core::Math::SimplexNoise noise;

			bool isInBounds(const glm::vec3& position) const;
			std::vector<glm::i32vec3> getAdjChunks(const Chunk& chunk);
			std::vector<glm::i32vec3> getAdjChunkRow(const Chunk& chunk, uint32_t height, bool excludeMiddle);
			std::vector<glm::i32vec3> getAdjCubes(const glm::i32vec3& cubePos);

			void generateChunkMesh(const Chunk& chunk, int& vertexCount);
			Voxel getVoxel(const glm::vec3& position);
	};
}
