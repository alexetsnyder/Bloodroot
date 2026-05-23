#pragma once

#include <iostream>

namespace Game
{
	enum class VoxelType
	{
		AIR,
		GRASS,
		DIRT,
		STONE,
		BEDROCK,
	};

	inline std::ostream& operator<<(std::ostream& os, const VoxelType& voxelType)
	{
		switch (voxelType)
		{
			case VoxelType::AIR:
				return os << "VoxelType::AIR";
			case VoxelType::GRASS:
				return os << "VoxelType::GRASS";
			case VoxelType::DIRT:
				return os << "VoxelType::DIRT";
			case VoxelType::STONE:
				return os << "VoxelType::STONE";
			case VoxelType::BEDROCK:
				return os << "VoxelType::BEDROCK";
			default:
				return os << "VoxelType::ERROR";
		}
	}

	struct Voxel
	{
		VoxelType Type;

		int frontFaceIndex;
		int backFaceIndex;
		int topFaceIndex;
		int bottomFaceIndex;
		int leftFaceIndex;
		int rightFaceIndex;
	};
}