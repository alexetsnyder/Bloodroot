#pragma once

#include "Mesh.h"

namespace Core::VK
{
	class Quad
	{
		public:
			static Mesh GenerateMesh()
			{
				auto mesh = Mesh{};

				auto x = 0;
				auto y = 0;

				mesh.AddVertex({ { x + 1, y + 1, 0 }, { 0.0f, 0.0f, 0.0f } });
				mesh.AddVertex({ { x, y + 1, 0 }, { 1.0f, 0.0f, 0.0f } });
				mesh.AddVertex({ { x + 1, y, 0 }, { 0.0f, 1.0f, 0.0f } });
				mesh.AddVertex({ { x, y, 0 }, { 1.0f, 1.0f, 0.0f } });

				return mesh;
			}

		private:
	};
}