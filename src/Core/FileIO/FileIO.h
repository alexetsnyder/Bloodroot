#pragma once

#include <string>
#include <vector>

namespace Core
{
	class FileIO
	{
		public:
			static std::vector<char> readFile(const std::string& fileName);

		private:
	};
}