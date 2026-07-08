#pragma once

#include "Common.h"

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

	struct PoissonStartInfo
	{
		glm::i32vec2 startPos;
		float radius;
		uint32_t k;
		float width;
		float height;
	};

	struct PoissonInfo
	{
		float cellSize;
		int32_t nCellsWidth;
		int32_t nCellsHeight;
		glm::i32vec2 startPos;
		float radius;
		float width;
		float height;
	};

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

			void PoissonDiskSampling(const PoissonStartInfo& data, std::vector<glm::vec2>& outPoints)
			{
				const auto& width = data.width;
				const auto& height = data.height;
				const auto& radius = data.radius;
				const auto& k = data.k;
				const auto& startPos = data.startPos;

				auto active = std::vector<glm::vec2>{};

				auto p0 = glm::vec2{ RandomFloat(startPos.x, width), RandomFloat(startPos.y, height) };

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

				auto info = PoissonInfo
				{
					.cellSize = cellSize,
					.nCellsWidth = nCellsWidth,
					.nCellsHeight = nCellsHeight,
					.startPos = startPos,
					.radius = radius, 
					.width = width,
					.height = height
				};

				insertPoint(info, p0, grid);
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

						if (!isValidPoint(grid, pNew, info))
						{
							continue;
						}

						outPoints.push_back(pNew);
						insertPoint(info, pNew, grid);
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
							  const glm::vec2& p,
							  const PoissonInfo& info)
			{
				if (p.x < info.startPos.x || p.x >= info.startPos.x + info.width ||
					p.y < info.startPos.y || p.y >= info.startPos.y + info.height)
				{
					return false;
				}

				auto xPrime = MapToPositiveRange(std::floor(p.x), info.startPos.x);
				auto yPrime = MapToPositiveRange(std::floor(p.y), info.startPos.y);

				auto xIndex = static_cast<int32_t>(std::floor(xPrime / info.cellSize));
				auto yIndex = static_cast<int32_t>(std::floor(yPrime / info.cellSize));

				auto i0 = std::max(xIndex - 1, 0);
				auto i1 = std::min(xIndex + 1, info.nCellsWidth - 1);
				auto j0 = std::max(yIndex - 1, 0);
				auto j1 = std::min(yIndex + 1, info.nCellsHeight - 1);

				for (int i = i0; i <= i1; i++)
				{
					for (int j = j0; j <= j1; j++)
					{
						if (grid[i][j] && Distance(grid[i][j].value(), p) < info.radius)
						{
							return false;
						}
					}
				}

				return true;
			}

			void insertPoint(const PoissonInfo& info, const glm::vec2& point, std::vector<std::vector<std::optional<glm::vec2>>>& outGrid)
			{
				auto xPrime = MapToPositiveRange(std::floor(point.x), info.startPos.x);
				auto yPrime = MapToPositiveRange(std::floor(point.y), info.startPos.y);

				auto xIndex = static_cast<uint32_t>(std::floor(xPrime / info.cellSize));
				auto yIndex = static_cast<uint32_t>(std::floor(yPrime / info.cellSize));
				outGrid[xIndex][yIndex] = point;
			}
	};
}