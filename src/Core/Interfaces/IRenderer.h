#pragma once

namespace Core
{
	class IRenderer
	{
		public:
			virtual void onResize(int width, int height) = 0;

			virtual ~IRenderer() = default;
	};
}
