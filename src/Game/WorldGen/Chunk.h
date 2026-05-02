#pragma once

#include "Mesh.h"
#include "Voxel.h"

#include <glm/glm.hpp>

namespace Game
{
	class Chunk
	{
		public:
			const int VOXEL_SIZE = 1;

			Chunk(int width, int height, int depth, glm::vec3 position);
			~Chunk();

			const Core::Mesh& GetMesh() { return mesh; }

			void generateMesh();

		private:
			int width, height, depth;
			glm::vec3 position;
			Core::Mesh mesh;

			void generateVoxel(const glm::vec3& voxelPos, int& vertexCount);
			Voxel getVoxel(int y);
	};
}