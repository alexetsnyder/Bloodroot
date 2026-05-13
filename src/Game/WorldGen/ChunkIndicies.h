#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace Game
{
	class ChunkIndicies
	{
		public:
			ChunkIndicies(const glm::u32vec3 chunkSize)
			{
				generateIndicies(chunkSize);
			}

			~ChunkIndicies() {}

			const std::vector<uint32_t>& Indicies() { return indicies; }

		private:
			std::vector<uint32_t> indicies;

			void generateIndicies(const glm::u32vec3& chunkSize)
			{
				uint32_t vertexCount = 0;
				for (uint32_t y = 0; y < chunkSize.y; y++)
				{
					for (uint32_t x = 0; x < chunkSize.x; x++)
					{
						for (uint32_t z = 0; z < chunkSize.z; z++)
						{
							for (int i = 0; i < 6; i++)
							{
								indicies.push_back(vertexCount);
								indicies.push_back(vertexCount + 1);
								indicies.push_back(vertexCount + 2);

								indicies.push_back(vertexCount + 2);
								indicies.push_back(vertexCount + 1);
								indicies.push_back(vertexCount + 3);

								vertexCount += 4;
							}
						}
					}
				}
			}
	};
}