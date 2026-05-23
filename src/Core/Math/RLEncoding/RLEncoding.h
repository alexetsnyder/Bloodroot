#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

namespace Core::Math
{
	class RLEncoding
	{
		public:
			RLEncoding() = delete;

			template <typename T>
			static void Print(std::vector<T> row)
			{
				std::cout << "[";
				for (int i = 0; i < row.size(); i++)
				{
					if (i == row.size() - 1)
					{
						std::cout << row[i];
					}
					else
					{
						std::cout << row[i] << ", ";
					}
				}
				std::cout << "]\n";
			}

			template <typename T>
			static T GetData(const std::vector<uint16_t>& encodedData, uint8_t index)
			{
				uint16_t runLength = 0;
				for (int i = 0; i < encodedData.size(); i++)
				{
					auto encodedValue = encodedData[i];
					runLength += (encodedValue >> 8) & 0xFF;

					if (runLength > index)
					{
						return static_cast<T>(encodedValue & 0xFF);
					}
				}

				return T{};
			}

			template <typename T>
			static void Encode(const std::vector<T>& dataTypes, std::vector<uint16_t>& encodedData)
			{
				auto currentData = dataTypes[0];
				uint8_t currentRun = 1;

				for (int i = 1; i < dataTypes.size(); i++)
				{
					auto nextData = dataTypes[i];
					if (nextData != currentData)
					{
						encodeRun(currentRun, currentData, encodedData);
						currentData = nextData;
						currentRun = 1;
					}
					else
					{
						currentRun++;
					}
				}

				encodeRun(currentRun, currentData, encodedData);
			}

			template <typename T>
			static std::vector<T> Decode(const std::vector<uint16_t> encodedData)
			{
				auto dataTypes = std::vector<T>{};

				for (int i = 0; i < encodedData.size(); i++)
				{
					auto encodedValue = encodedData[i];
					auto run = (encodedValue >> 8) & 0xFF;
					auto dataType = static_cast<T>(encodedValue & 0xFF);
					for (int j = 0; j < run; j++)
					{
						dataTypes.push_back(dataType);
					}
				}

				return dataTypes;
			}

		private:
			template <typename T>
			static void encodeRun(uint16_t run, T data, std::vector<uint16_t>& encodedData)
			{
				uint16_t encodedValue = (static_cast<uint16_t>(run) << 8) | static_cast<uint16_t>(data);
				encodedData.push_back(encodedValue);
			}
	};
}