#pragma once

#include "Chunk.h"
#include "SimplexNoise.h"
#include "Voxel.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Game
{
	struct ChunkMesh
	{
		Chunk chunk;
		Core::Mesh mesh;
	};

	class MeshGen
	{
		public:
			const uint32_t MIN_HEIGHT = 32;
			const uint32_t VARY_HEIGHT = 16;
			const uint32_t DIRT_DEPTH = 3;

			MeshGen(const glm::i32vec3& worldCenter, const glm::u32vec3& worldSize, const glm::u32vec3& chunkSize);

			void GenerateChunkMeshes(std::vector<ChunkMesh>& chunkMeshes);

		private:
			glm::u32vec3 worldSize;
			glm::i32vec3 worldCenter;
			glm::u32vec3 chunkSize;
			Core::Math::SimplexNoise noise;

			bool isInBounds(const glm::vec3& position) const;
			std::vector<glm::i32vec3> getAdjChunks(const ChunkMesh& chunkMesh);
			std::vector<glm::i32vec3> getAdjChunkRow(const ChunkMesh& chunkMesh, uint32_t height, bool excludeMiddle);
			std::vector<glm::i32vec3> getAdjCubes(const glm::i32vec3& cubePos);

			void generateChunkMesh(std::vector<ChunkMesh>& chunkMeshes, uint32_t& vertexCount);
			Voxel getVoxel(const glm::vec3& position);
	};
}
