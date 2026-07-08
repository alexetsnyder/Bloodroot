#include "Common.h"



#include <assert.h>

namespace Core::Math
{
	uint32_t MapToPositiveRange(int32_t value, int32_t start)
	{
		return std::abs(value - start);
	}

	float Distance(const glm::vec2& p1, const glm::vec2& p2)
	{
		return Distance(p1.x, p2.x, p1.y, p2.y);
	}

	float Distance(float x1, float x2, float y1, float y2)
	{
		return std::sqrt(DistanceSquared(x1, x2, y1, y2));
	}

	float DistanceSquared(float x1, float x2, float y1, float y2)
	{
		float xDiff = x2 - x1;
		float yDiff = y2 - y1;

		return ((xDiff * xDiff) + (yDiff * yDiff));
	}	
}