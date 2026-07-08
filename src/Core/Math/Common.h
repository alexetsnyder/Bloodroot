#pragma once

#include <glm/glm.hpp>

#include <cstdint>

namespace Core::Math
{
	uint32_t MapToPositiveRange(int32_t value, int32_t start);

	float Distance(const glm::vec2& p1, const glm::vec2& p2);

	float Distance(float x1, float x2, float y1, float y2);

	float DistanceSquared(float x1, float x2, float y1, float y2);
}