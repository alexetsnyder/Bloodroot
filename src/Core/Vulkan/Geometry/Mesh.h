#pragma once

#include "Vertex.h"

#include <vector>

namespace Core::VK
{
	class Mesh
	{
		public:
			Mesh() {}
			~Mesh() {}

			Mesh(const Mesh& other)
			{
				indexCount = other.indexCount;

				for (auto& vertex : other.verticies)
				{
					verticies.push_back(vertex);
				}

				for (auto& index : other.indicies)
				{
					indicies.push_back(index);
				}
			}

			Mesh(Mesh&& other) noexcept
			{
				indexCount = other.indexCount;

				for (auto& vertex : other.verticies)
				{
					verticies.push_back(vertex);
				}

				for (auto& index : other.indicies)
				{
					indicies.push_back(index);
				}

				other.indexCount = 0;
				other.verticies.clear();
				other.indicies.clear();
			}

			const std::vector<Vertex>& Verticies() const { return verticies; }
			const std::vector<uint32_t>& Indicies() const { return indicies; }

			void AddVertex(const Vertex& vertex) { verticies.push_back(vertex); }
			void AddIndex(int index) { indicies.push_back(index); }

			uint32_t& IndexCount() { return indexCount; }
			uint32_t ConstIndexCount() const { return indexCount; }
			bool IsEmpty() const { return indexCount <= 0; }

		private:
			uint32_t indexCount = 0;
			std::vector<Vertex> verticies;
			std::vector<uint32_t> indicies;
	};
}