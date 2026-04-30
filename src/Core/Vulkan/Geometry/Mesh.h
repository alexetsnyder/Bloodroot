#pragma once

#include "Vertex.h"

#include <vector>

namespace Core
{
	class Mesh
	{
		public:
			Mesh() {}
			~Mesh() {}

			const std::vector<Vertex>& Verticies() const { return verticies; }
			const std::vector<uint16_t>& Indicies() const { return indicies; }

			void AddVertex(const Vertex& vertex) { verticies.push_back(vertex); }
			void AddIndex(int index) { indicies.push_back(index); }

		private:
			std::vector<Vertex> verticies;
			std::vector<uint16_t> indicies;
	};
}