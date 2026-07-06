#pragma once

#include <array>
#include <chrono>
#include <random>

namespace Core::Math
{
	class Random
	{
		public:
			static Random& Instance()
			{
				static Random instance;
				return instance;
			}

			uint32_t RandomInt(uint32_t min, uint32_t max)
			{
				return std::uniform_int_distribution{ min, max }(mt);
			}

		private:
			std::array<uint32_t, 8> seedData;
			std::seed_seq seedSeq;
			std::mt19937 mt;

			static std::array<uint32_t, 8> getSeedData()
			{
				std::random_device rd{};
				std::array<uint32_t, 8> data{};

				for (auto& seed : data)
				{
					seed = rd();
				}

				return data;
			}

			Random() 
				: seedData{ getSeedData() },
				  seedSeq{ seedData.begin(), seedData.end() },
				  mt{ seedSeq }
			{

			};

			~Random() {};
	};
}