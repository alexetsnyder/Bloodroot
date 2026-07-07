#pragma once

#include <glm/glm.hpp>

#include <cmath>

namespace Core::Math
{
	class Geometry
	{
		public:
			static float Distance(const glm::vec2& p1, const glm::vec2& p2)
			{
				return Distance(p1.x, p2.x, p1.y, p2.y);
			}

			static float Distance(float x1, float x2, float y1, float y2)
			{
				return std::sqrt(DistanceSquared(x1, x2, y1, y2));
			}

			static float DistanceSquared(float x1, float x2, float y1, float y2)
			{
				float xDiff = x2 - x1;
				float yDiff = y2 - y1;

				return ((xDiff * xDiff) + (yDiff * yDiff));
			}
		private:
	};
}