#pragma once

#include "FastNoiseLite.h"

#include <glm/glm.hpp>

namespace Core::Math
{
	class SimplexNoise
	{
		public:
			SimplexNoise(uint32_t seed);

			float GetNoise(float x, float z);

		private:
			FastNoiseLite noise;
	};
}