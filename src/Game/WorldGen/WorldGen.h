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
	struct BuildInfo
	{
		glm::i32vec3 startPos;
		glm::i32vec3 endPos;
		glm::i32vec3 size;
	};

	class WorldGen
	{
		public:
			WorldGen();
			WorldGen(const glm::vec3& center);
			~WorldGen();

			void GenerateChunks(const BuildInfo& buildInfo, std::unordered_map<glm::i32vec3, Chunk>& chunks);
			void GenerateMeshes(const BuildInfo& buildInfo,
								const std::unordered_map<glm::i32vec3, Chunk>& chunks,
								std::unordered_map<glm::i32vec3, Core::VK::Mesh>& meshes,
								std::unordered_map<glm::i32vec3, Core::VK::Mesh>& tMeshes);

			bool IsInBounds(const BuildInfo& buildInfo, const glm::vec3& position) const;

		private:
			glm::vec3 worldCenter;
			TerrainGen terrainGen;

			size_t getIndex(const BuildInfo& buildInfo, const glm::i32vec3& cubePos) const;

			void generateChunk(const glm::i32vec3& position, std::unordered_map<glm::i32vec3, Chunk>& chunks);

			void getAdjCubes(const glm::i32vec3& cubePos, glm::i32vec3 adjCubes[]);
			Voxel getVoxel(const BuildInfo& buildInfo,
						   const std::unordered_map<glm::i32vec3,
						   Chunk>& chunks,
						   const glm::i32vec3& cubePos);
	};
}