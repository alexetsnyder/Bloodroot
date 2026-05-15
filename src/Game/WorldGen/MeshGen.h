#pragma once

#include "Chunk.h"
#include "IRenderable.h"
#include "SimplexNoise.h"
#include "Voxel.h"

#include <glm/glm.hpp>

#include <memory>
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

			void GenerateChunkMeshes();

			std::vector<Chunk>& Chunks() { return chunks; }
			std::vector<std::shared_ptr<Core::IRenderable>> Renderables();

		private:
			glm::u32vec3 worldSize;
			glm::i32vec3 worldCenter;
			glm::u32vec3 chunkSize;
			std::vector<Chunk> chunks;
			Core::Math::SimplexNoise noise;

			bool isInBounds(const glm::vec3& position) const;
			std::vector<glm::i32vec3> getAdjChunks(const Chunk& chunk);
			std::vector<glm::i32vec3> getAdjChunkRow(const Chunk& chunk, uint32_t height, bool excludeMiddle);
			std::vector<glm::i32vec3> getAdjCubes(const glm::i32vec3& cubePos);

			void generateChunkMesh(Chunk& chunk, int& vertexCount);
			Voxel getVoxel(const glm::vec3& position);
	};
}
