#pragma once

#include <iostream>

namespace Game
{
	enum class VoxelType
	{
		AIR,
		WATER,
		OAK_LOG,
		OAK_LEAVES,
		GRASS,
		DIRT,
		SAND,
		STONE,
		BEDROCK,
	};

	inline std::ostream& operator<<(std::ostream& os, const VoxelType& voxelType)
	{
		switch (voxelType)
		{
			case VoxelType::AIR:
				return os << "VoxelType::AIR";
			case VoxelType::WATER:
				return os << "VoxelType::WATER";
			case VoxelType::OAK_LOG:
				return os << "VoxelType::OAK_LOG";
			case VoxelType::OAK_LEAVES:
				return os << "VoxelType::OAK_LEAVES";
			case VoxelType::GRASS:
				return os << "VoxelType::GRASS";
			case VoxelType::DIRT:
				return os << "VoxelType::DIRT";
			case VoxelType::SAND:
				return os << "VoxelType::SAND";
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