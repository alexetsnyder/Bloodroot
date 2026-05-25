#pragma once

#include "Chunk.h"
#include "GLMExtensions.h"
#include "TerrainGen.h"

#include <glm/glm.hpp>

#include <map>
#include <vector>

namespace Game
{
	struct ChunkMesh
	{
		glm::i32vec3 chunkId;
		Core::Mesh mesh;
	};

	class WorldGen
	{
		public:
			WorldGen();
			WorldGen(const glm::vec3& center, const glm::vec3& size);
			~WorldGen();

			void GenerateChunks(std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks);
			void GenerateMeshes(const std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks, 
								std::vector<ChunkMesh>& chunkMeshes);

			bool IsInBounds(const glm::vec3& position) const;

		private:
			glm::vec3 worldCenter;
			glm::u32vec3 worldSize;
			TerrainGen terrainGen;

			void generateChunk(const glm::i32vec3& position, std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks);

			std::vector<glm::i32vec3> getAdjCubes(const glm::i32vec3& cubePos);
			Voxel getVoxel(const std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks,
						   const glm::i32vec3& cubePos);
			void generateMesh(const std::map<glm::i32vec3, Chunk, Core::Ext::I32Vec3Comparator>& chunks,
							  const Chunk& chunk, std::vector<ChunkMesh>& chunkMeshes);
	};
}