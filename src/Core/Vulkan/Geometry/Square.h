#pragma once

#include "Vertex.h"

#include <vector>

namespace Core
{
	class Square
	{
		public:
			static const inline std::vector<Vertex> vertices
			{
				{{-0.5f, -0.5f, 0.0f}, { 1.0f, 0.0f }},
				{{0.5f, -0.5f, 0.0f},  { 0.0f, 0.0f }},
				{{0.5f, 0.5f, 0.0f},   { 0.0f, 1.0f }},
				{{-0.5f, 0.5f, 0.0f},  { 1.0f, 1.0f }},
			};

			static const inline std::vector<uint16_t> indices
			{
				0, 1, 2,
				2, 3, 0,
			};
	};
}