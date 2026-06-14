#pragma once

#include <glm/glm.hpp>

namespace Core::Ext
{
	struct I32Vec3Comparator
	{
		bool operator()(const glm::i32vec3& lhs, const glm::i32vec3& rhs) const
		{
			return (std::tie(lhs.x, lhs.y, lhs.z) < std::tie(rhs.x, rhs.y, rhs.z));
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const glm::i32vec3& vector)
	{
		os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";

		return os;
	}
}