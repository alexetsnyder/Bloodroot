#pragma once

#include "Mesh.h"

#include <glm/glm.hpp>

namespace Game
{
	class Chunk
	{
		public:
			Chunk(int width, int height, int depth, glm::vec3 position);
			~Chunk();

			const Core::Mesh& GetMesh() { return mesh; }

			void generateMesh();

		private:
			int width, height, depth;
			glm::vec3 position;
			Core::Mesh mesh;

			void generateVoxel(int x, int y, int z);
	};
}