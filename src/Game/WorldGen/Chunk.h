#pragma once

#include "Mesh.h"
#include "Voxel.h"

#include <glm/glm.hpp>

namespace Game
{
	enum class CubeFace
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		FRONT,
		BACK,
	};

	class Chunk
	{
		public:
			const int VOXEL_SIZE = 1;

			Chunk(int width, int height, int depth, glm::vec3 position);
			~Chunk();

			bool IsSolid(const glm::vec3& position);
			
			const Core::Mesh& GetMesh() { return mesh; }

			void generateMesh();
			void generateMesh2();

		private:
			int width, height, depth;
			glm::vec3 position;
			Core::Mesh mesh;

			bool IsSolid(const glm::i32vec3& cubePos);
			bool IsInBounds(const glm::i32vec3& cubePos);
			std::vector<glm::i32vec3> getAdjCubes(const glm::i32vec3& cubePos);
			void createFace(CubeFace face, const glm::i32vec3& cubePos, int& vertexCount);

			glm::vec3 mapToLocal(const glm::vec3& position);
			void generateVoxel(const glm::vec3& voxelPos, int& vertexCount);
			Voxel getVoxel(int y);
	};
}