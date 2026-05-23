#pragma once

#include <glm/glm.hpp>

namespace Game
{
	class WorldGen
	{
		public:
			WorldGen();
			WorldGen(const glm::vec3& center, const glm::vec3& size);
			~WorldGen();

		private:
	};
}