#pragma once

namespace Game
{
	enum class VoxelType
	{
		AIR,
		GRASS,
		DIRT,
		STONE,
	};

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