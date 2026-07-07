#pragma once

#include "Geometry.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <optional>
#include <random>
#include <vector>

#include <glm/glm.hpp>

namespace Core::Math
{
	constexpr uint32_t N = 2;

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

			float RandomFloat(float min, float max)
			{
				return std::uniform_real_distribution{ min, max }(mt);
			}

			void PoissonDiskSampling(float radius, int k, float width, float height, std::vector<glm::vec2>& outPoints)
			{
				auto active = std::vector<glm::vec2>{};

				auto p0 = glm::vec2{ RandomFloat(0, width), RandomFloat(0, height) };

				float cellSize = std::floor(radius / std::sqrt(static_cast<float>(N)));
				auto nCellsWidth = static_cast<int32_t>(std::ceil(width / cellSize)) + 1;
				auto nCellsHeight = static_cast<int32_t>(std::ceil(height / cellSize)) + 1;

				auto grid = std::vector<std::vector<std::optional<glm::vec2>>>{};
				for (int i = 0; i < nCellsWidth; i++)
				{
					grid.push_back({});
					for (int j = 0; j < nCellsHeight; j++)
					{
						grid[i].push_back({});
					}
				}

				insertPoint(cellSize, p0, grid);
				outPoints.push_back(p0);
				active.push_back(p0);

				while (!active.empty())
				{
					auto randomIndex = RandomInt(0, active.size() - 1);
					const auto& p = active[randomIndex];

					bool isFound = false;
					for (int tries = 0; tries < k; tries++)
					{
						auto theta = RandomFloat(0.0f, 360.0f);
						auto newRadius = RandomFloat(radius, 2 * radius);
						auto pNewX = p.x + newRadius * std::cos(glm::radians(theta));
						auto pNewY = p.y + newRadius * std::sin(glm::radians(theta));
						auto pNew = glm::vec2{ pNewX, pNewY };

						if (!isValidPoint(grid, cellSize, nCellsWidth, nCellsHeight, pNew, radius, width, height))
						{
							continue;
						}

						outPoints.push_back(pNew);
						insertPoint(cellSize, pNew, grid);
						active.push_back(pNew);
						isFound = true;
						break;
					}

					if (!isFound)
					{
						active.erase(active.begin() + randomIndex);
					}
				}
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

			bool isValidPoint(const std::vector<std::vector<std::optional<glm::vec2>>>& grid,
				float cellSize, int32_t gWidth, int32_t gHeight, const glm::vec2& p,
				float radius, float width, float height)
			{
				if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height)
				{
					return false;
				}

				auto xIndex = static_cast<int32_t>(std::floor(p.x / cellSize));
				auto yIndex = static_cast<int32_t>(std::floor(p.y / cellSize));

				auto i0 = std::max(xIndex - 1, 0);
				auto i1 = std::min(xIndex + 1, gWidth - 1);
				auto j0 = std::max(yIndex - 1, 0);
				auto j1 = std::min(yIndex + 1, gHeight - 1);

				for (int i = i0; i <= i1; i++)
				{
					for (int j = j0; j <= j1; j++)
					{
						if (grid[i][j] && Geometry::Distance(grid[i][j].value(), p) < radius)
						{
							return false;
						}
					}
				}

				return true;
			}

			void insertPoint(float cellSize, const glm::vec2& point, std::vector<std::vector<std::optional<glm::vec2>>>& outGrid)
			{
				auto xIndex = static_cast<uint32_t>(std::floor(point.x / cellSize));
				auto yIndex = static_cast<uint32_t>(std::floor(point.y / cellSize));
				outGrid[xIndex][yIndex] = point;
			}
	};
}