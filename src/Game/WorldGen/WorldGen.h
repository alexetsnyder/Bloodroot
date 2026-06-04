#pragma once

#include "Chunk.h"
#include "GLMExtensions.h"
#include "Mesh.h"
#include "TerrainGen.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <vector>

namespace Game
{
	struct ChunkMesh
	{
		glm::i32vec3 chunkId;
		Core::VK::Mesh mesh;
	};

	class WorldGen
	{
		public:
			WorldGen();
			WorldGen(const glm::vec3& center, const glm::vec3& size);
			~WorldGen();

			void GenerateChunks(std::unordered_map<glm::i32vec3, Chunk>& chunks);
			void GenerateMeshes(const std::unordered_map<glm::i32vec3, Chunk>& chunks,
								std::unordered_map<glm::i32vec3, Core::VK::Mesh>& meshes,
								std::unordered_map<glm::i32vec3, Core::VK::Mesh>& tMeshes);

			bool IsInBounds(const glm::vec3& position) const;

		private:
			glm::vec3 worldCenter;
			glm::u32vec3 worldSize;
			TerrainGen terrainGen;

			size_t getIndex(const glm::i32vec3& cubePos) const;

			void generateChunk(const glm::i32vec3& position, std::unordered_map<glm::i32vec3, Chunk>& chunks);

			void getAdjCubes(const glm::i32vec3& cubePos, glm::i32vec3 adjCubes[]);
			Voxel getVoxel(const std::unordered_map<glm::i32vec3, Chunk>& chunks,
						   const glm::i32vec3& cubePos);
	};
}